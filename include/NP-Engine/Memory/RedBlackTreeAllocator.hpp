//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/10/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RED_BLACK_TREE_ALLOCATOR_HPP
#define NP_ENGINE_RED_BLACK_TREE_ALLOCATOR_HPP

#include <limits>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "SizedAllocator.hpp"
#include "Margin.hpp"
#include "MemoryFunctions.hpp"

namespace np::memory
{
	namespace __detail
	{
		class RedBlackTreeAllocatorNode
		{
		private:
			using Node = RedBlackTreeAllocatorNode;
			using NodePtr = Node*;

			// we compress our parent and color value since our alignment enables us to do so, hence nullptr is okay
			NodePtr ParentAndColor = nullptr;

		public:
			NodePtr Left = nullptr;
			NodePtr Right = nullptr;

			NodePtr GetParent()
			{
				ui64 i;
				CopyBytes(&i, &ParentAndColor, sizeof(NodePtr));
				i = (i >> 1) << 1;

				NodePtr p;
				CopyBytes(&p, &i, sizeof(NodePtr));

				return p;
			}

			NodePtr GetParent() const
			{
				ui64 i;
				CopyBytes(&i, &ParentAndColor, sizeof(NodePtr));
				i = (i >> 1) << 1;

				NodePtr p;
				CopyBytes(&p, &i, sizeof(NodePtr));

				return p;
			}

			void SetParent(NodePtr parent)
			{
				if (IsRed())
				{
					ParentAndColor = parent;
					SetRed();
				}
				else
				{
					ParentAndColor = parent;
					SetBlack();
				}
			}

			bl IsRed() const
			{
				ui64 i;
				CopyBytes(&i, &ParentAndColor, sizeof(NodePtr));

				return i & 1;
			}

			bl IsBlack() const
			{
				return !IsRed();
			}

			void SetRed()
			{
				ui64 i;
				CopyBytes(&i, &ParentAndColor, sizeof(NodePtr));
				i |= 1;
				CopyBytes(&ParentAndColor, &i, sizeof(NodePtr));
			}

			void SetBlack()
			{
				ParentAndColor = GetParent();
			}

			siz GetSize() const
			{
				return ((MarginPtr)((ui8*)this - __detail::MARGIN_ALIGNED_SIZE))->GetSize();
			}
		};

		/**
		 rules:
		 1- root and leaves(nil's) are block
		 2- if node is red, the parent is black
		 3- all simple paths from any node x to descendent leaf have the same number of black nodes
		 */
		class RedBlackTreeAllocatorTree
		{
		public:
			using Node = RedBlackTreeAllocatorNode;
			using NodePtr = Node*;

		private:
			NodePtr _root;
			Margin _nil_margin; // required for _nil.GetSize()
			Node _nil;

			NodePtr GetMinFrom(NodePtr node)
			{
				NodePtr min = node;
				for (; min->Left != &_nil; min = min->Left)
					;
				return min;
			}

			/*
				transplants node b to a
			*/
			void Transplant(NodePtr a, NodePtr b)
			{
				Node* parent = a->GetParent();
				if (parent == nullptr)
				{
					_root = b;
				}
				else if (a == parent->Left)
				{
					parent->Left = b;
				}
				else
				{
					parent->Right = b;
				}
				b->SetParent(parent);
			}

			void RotateCCW(NodePtr node)
			{
				NodePtr parent = node->GetParent();
				NodePtr right = node->Right;

				node->Right = right->Left;
				if (right->Left != &_nil)
				{
					right->Left->SetParent(node);
				}

				right->SetParent(parent);
				if (parent == nullptr)
				{
					_root = right;
				}
				else if (parent->Left == node)
				{
					parent->Left = right;
				}
				else
				{
					parent->Right = right;
				}
				right->Left = node;
				node->SetParent(right);
			}

			void RotateCW(NodePtr node)
			{
				NodePtr parent = node->GetParent();
				NodePtr left = node->Left;

				node->Left = left->Right;
				if (left->Right != &_nil)
				{
					left->Right->SetParent(node);
				}

				left->SetParent(parent);
				if (parent == nullptr)
				{
					_root = left;
				}
				else if (parent->Left == node)
				{
					parent->Left = left;
				}
				else
				{
					parent->Right = left;
				}
				left->Right = node;
				node->SetParent(left);
			}

			void InsertCleanup(NodePtr node)
			{
				while (node->GetParent() != nullptr && node->GetParent()->IsRed())
				{
					NodePtr y = nullptr;
					if (node->GetParent() == node->GetParent()->GetParent()->Left)
					{
						y = node->GetParent()->GetParent()->Right;

						if (y->IsRed())
						{
							node->GetParent()->SetBlack();
							y->SetBlack();
							node->GetParent()->GetParent()->SetRed();
							node = node->GetParent()->GetParent();
						}
						else
						{
							if (node == node->GetParent()->Right)
							{
								node = node->GetParent();
								RotateCCW(node);
							}

							node->GetParent()->SetBlack();
							node->GetParent()->GetParent()->SetRed();
							RotateCW(node->GetParent()->GetParent());
						}
					}
					else
					{
						y = node->GetParent()->GetParent()->Left;

						if (y->IsRed())
						{
							node->GetParent()->SetBlack();
							y->SetBlack();
							node->GetParent()->GetParent()->SetRed();
							node = node->GetParent()->GetParent();
						}
						else
						{
							if (node == node->GetParent()->Left)
							{
								node = node->GetParent();
								RotateCW(node);
							}

							node->GetParent()->SetBlack();
							node->GetParent()->GetParent()->SetRed();
							RotateCCW(node->GetParent()->GetParent());
						}
					}
				}

				_root->SetBlack();
			}

			void RemoveCleanup(NodePtr node)
			{
				while (node != _root && node->IsBlack())
				{
					NodePtr temp = nullptr;
					if (node->GetParent()->Left == node)
					{
						temp = node->GetParent()->Right;
						if (temp->IsRed())
						{
							temp->SetBlack();
							node->GetParent()->SetRed();
							RotateCCW(node->GetParent());
							temp = node->GetParent()->Right;
						}
						if ((temp->Right == &_nil || temp->Right->IsBlack()) && (temp->Left == &_nil || temp->Left->IsBlack()))
						{
							temp->SetRed();
							node = node->GetParent();
						}
						else
						{
							if (temp->Right == &_nil || temp->Right->IsBlack())
							{
								temp->Left->SetBlack();
								temp->SetRed();
								RotateCW(temp);
								temp = node->GetParent()->Right;
							}

							if (node->GetParent()->IsBlack())
							{
								temp->SetBlack();
							}
							else
							{
								temp->SetRed();
							}

							node->GetParent()->SetBlack();
							temp->Right->SetBlack();
							RotateCCW(node->GetParent());
							node = _root;
						}
					}
					else
					{
						temp = node->GetParent()->Left;
						if (temp->IsRed())
						{
							temp->SetBlack();
							node->GetParent()->SetRed();
							RotateCW(node->GetParent());
							temp = node->GetParent()->Left;
						}
						if ((temp->Right == &_nil || temp->Right->IsBlack()) && (temp->Left == &_nil || temp->Left->IsBlack()))
						{
							temp->SetRed();
							node = node->GetParent();
						}
						else
						{
							if (temp->Left == &_nil || temp->Left->IsBlack())
							{
								temp->Right->SetBlack();
								temp->SetRed();
								RotateCCW(temp);
								temp = node->GetParent()->Left;
							}

							if (node->GetParent()->IsBlack())
							{
								temp->SetBlack();
							}
							else
							{
								temp->SetRed();
							}

							node->GetParent()->SetBlack();
							temp->Left->SetBlack();
							RotateCW(node->GetParent());
							node = _root;
						}
					}
				}
				node->SetBlack();
			}

		public:
			RedBlackTreeAllocatorTree(): _root(nullptr)
			{
				Clear();
			}

			NodePtr SearchBest(siz size)
			{
				NodePtr found = nullptr;
				siz min_diff = ::std::numeric_limits<siz>::max();
				for (NodePtr it = _root; it != &_nil;)
				{
					siz diff = it->GetSize() - size;

					if (diff == 0)
					{
						found = it;
						break;
					}
					else if (diff < min_diff)
					{
						found = it;
						min_diff = diff;
					}

					if (it->GetSize() > size)
					{
						it = it->Left;
					}
					else
					{
						it = it->Right;
					}
				}
				return found;
			}

			NodePtr SearchFirst(siz size)
			{
				NodePtr found = nullptr;
				for (NodePtr it = _root; it != &_nil; it = it->Right)
				{
					if (it->GetSize() >= size)
					{
						found = it;
						break;
					}
				}
				return found;
			}

			void Insert(NodePtr node)
			{
				NodePtr parent = nullptr;

				if (node != _root)
				{
					for (NodePtr it = _root; it != &_nil; it = node->GetSize() < it->GetSize() ? it->Left : it->Right)
					{
						parent = it;
					}
				}

				node->SetParent(parent);

				if (parent == nullptr)
				{
					_root = node;
				}
				else if (node->GetSize() < parent->GetSize())
				{
					parent->Left = node;
				}
				else
				{
					parent->Right = node;
				}

				node->Left = &_nil;
				node->Right = &_nil;
				node->SetRed();

				InsertCleanup(node);
			}

			void Remove(NodePtr node)
			{
				NodePtr successor = nullptr;
				bl successor_is_black = node->IsBlack();

				if (node->Left == &_nil)
				{
					successor = node->Right;
					Transplant(node, successor);
				}
				else if (node->Right == &_nil)
				{
					successor = node->Left;
					Transplant(node, successor);
				}
				else
				{
					NodePtr min = GetMinFrom(node->Right);
					successor_is_black = min->IsBlack();
					successor = min->Right;

					if (min->GetParent() == node)
					{
						successor->SetParent(min);
					}
					else
					{
						Transplant(min, min->Right);
						min->Right = node->Right;
						if (min->Right != &_nil)
						{
							min->Right->SetParent(min);
						}
					}

					Transplant(node, min);
					min->Left = node->Left;
					if (min->Left != &_nil)
					{
						min->Left->SetParent(min);
					}

					if (node->IsBlack())
					{
						min->SetBlack();
					}
					else
					{
						min->SetRed();
					}
				}

				if (successor_is_black)
				{
					RemoveCleanup(successor);
				}
			}

			void Clear()
			{
				_nil.Left = nullptr;
				_nil.Right = nullptr;
				_nil.SetParent(nullptr);
				_nil.SetBlack();
				NP_ASSERT(_nil.GetSize() == 0, "our nil size must always be 0");
				_root = &_nil;
			}
		};
	} // namespace __detail

	class RedBlackTreeAllocator : public SizedAllocator
	{
	private:
		using Tree = __detail::RedBlackTreeAllocatorTree;
		using Node = Tree::Node;
		using NodePtr = Tree::NodePtr;
		using Margin = __detail::Margin;
		using MarginPtr = __detail::MarginPtr;

		constexpr static siz NODE_ALIGNED_SIZE = CalcAlignedSize(sizeof(Node));
		constexpr static siz OVERHEAD_ALIGNED_SIZE = (__detail::MARGIN_ALIGNED_SIZE << 1) + NODE_ALIGNED_SIZE;

		Mutex _mutex;
		Tree _tree;

		bl InitFreeBlock(Block& block)
		{
			bl initialized = false;

			if (block.size >= OVERHEAD_ALIGNED_SIZE)
			{
				Block header_block{block.Begin(), __detail::MARGIN_ALIGNED_SIZE};
				Block footer_block{(ui8*)block.End() - __detail::MARGIN_ALIGNED_SIZE, __detail::MARGIN_ALIGNED_SIZE};
				Construct<Margin>(header_block);
				Construct<Margin>(footer_block);
				MarginPtr header = (MarginPtr)header_block.Begin();
				MarginPtr footer = (MarginPtr)footer_block.Begin();
				header->SetSize(block.size);
				header->SetDeallocated();
				footer->Value = header->Value;

				Block node_block{header_block.End(), NODE_ALIGNED_SIZE};
				Construct<Node>(node_block);

				initialized = true;
			}
			else
			{
				block.Zeroize();
			}

			return initialized;
		}

		void StowFreeBlock(Block& block)
		{
			bl init_success = InitFreeBlock(block);
			NP_ASSERT(init_success, "our init here should always succeed");

			NodePtr node = (NodePtr)((ui8*)block.Begin() + __detail::MARGIN_ALIGNED_SIZE);
			_tree.Insert(node);
		}

		ui8* FindAllocation(siz required_aligned_size, bl true_best_false_first)
		{
			ui8* allocation = nullptr;

			if (true_best_false_first)
			{
				NodePtr alloc_node = _tree.SearchBest(required_aligned_size);
				if (alloc_node != nullptr)
				{
					allocation = (ui8*)alloc_node - __detail::MARGIN_ALIGNED_SIZE;
				}
			}
			else
			{
				NodePtr alloc_node = _tree.SearchFirst(required_aligned_size);
				if (alloc_node != nullptr)
				{
					allocation = (ui8*)alloc_node - __detail::MARGIN_ALIGNED_SIZE;
				}
			}

			return allocation;
		}

		Block Allocate(siz size, bl true_best_false_first)
		{
			Lock lock(_mutex);
			Block block;
			siz required_alloc_size = CalcAlignedSize(size) + (__detail::MARGIN_ALIGNED_SIZE << 1);
			if (required_alloc_size < OVERHEAD_ALIGNED_SIZE)
			{
				required_alloc_size = OVERHEAD_ALIGNED_SIZE;
			}
			ui8* alloc = FindAllocation(required_alloc_size, true_best_false_first);

			if (alloc != nullptr)
			{
				MarginPtr header = (MarginPtr)alloc;
				Block alloc_block{header, header->GetSize()};

				NodePtr node = (NodePtr)((ui8*)alloc + __detail::MARGIN_ALIGNED_SIZE);
				_tree.Remove(node);

				// can we split?
				if (header->GetSize() - required_alloc_size >= OVERHEAD_ALIGNED_SIZE)
				{
					Block split_block{alloc + required_alloc_size, header->GetSize() - required_alloc_size};
					alloc_block.size -= split_block.size;
					StowFreeBlock(split_block);

					bl alloc_success = InitFreeBlock(alloc_block);
					NP_ASSERT(alloc_success, "alloc_success must happen here");
				}

				header->SetAllocated();
				MarginPtr footer = (MarginPtr)((ui8*)alloc_block.End() - __detail::MARGIN_ALIGNED_SIZE);
				footer->Value = header->Value;

				block.ptr = (ui8*)alloc_block.Begin() + __detail::MARGIN_ALIGNED_SIZE;
				block.size = alloc_block.size - (__detail::MARGIN_ALIGNED_SIZE << 1);
			}

			return block;
		}

		void Init()
		{
			InitFreeBlock(_block);

			MarginPtr header = (MarginPtr)_block.Begin();
			MarginPtr footer = (MarginPtr)((ui8*)_block.End() - __detail::MARGIN_ALIGNED_SIZE);
			header->SetSize(_block.size);
			header->SetDeallocated();
			footer->Value = header->Value;

			NodePtr root = (NodePtr)((ui8*)_block.Begin() + __detail::MARGIN_ALIGNED_SIZE);
			_tree.Clear();
			_tree.Insert(root);
		}

	public:
		RedBlackTreeAllocator(Block block): SizedAllocator(block)
		{
			Init();
		}

		RedBlackTreeAllocator(siz size): SizedAllocator(size)
		{
			Init();
		}

		Block Allocate(siz size) override
		{
			return AllocateBest(size);
		}

		Block AllocateBest(siz size)
		{
			return Allocate(size, true);
		}

		Block AllocateFirst(siz size)
		{
			return Allocate(size, false);
		}

		bl Deallocate(Block& block) override
		{
			bl deallocated = Deallocate(block.ptr);
			if (deallocated)
			{
				block.Invalidate();
			}
			return deallocated;
		}

		bl Deallocate(void* ptr) override
		{
			Lock lock(_mutex);
			bl deallocated = false;

			if (Contains(ptr))
			{
				MarginPtr header = (MarginPtr)((ui8*)ptr - __detail::MARGIN_ALIGNED_SIZE);
				header->SetDeallocated();

				// coalesce
				{
					for (void* prev_footer = (ui8*)header - __detail::MARGIN_ALIGNED_SIZE; Contains(prev_footer);
						 prev_footer = (ui8*)header - __detail::MARGIN_ALIGNED_SIZE)
					{
						if (((MarginPtr)prev_footer)->IsAllocated())
						{
							break;
						}
						else
						{
							MarginPtr claim_header = (MarginPtr)((ui8*)prev_footer + __detail::MARGIN_ALIGNED_SIZE -
																 ((MarginPtr)prev_footer)->GetSize());

							NodePtr claim_node = (NodePtr)((ui8*)claim_header + __detail::MARGIN_ALIGNED_SIZE);
							_tree.Remove(claim_node);

							claim_header->SetSize(claim_header->GetSize() + header->GetSize());
							MarginPtr claim_footer =
								(MarginPtr)((ui8*)claim_header + claim_header->GetSize() - __detail::MARGIN_ALIGNED_SIZE);
							claim_footer->Value = claim_header->Value;
							header = claim_header;
						}
					}

					for (void* next_header = (ui8*)header + header->GetSize(); Contains(next_header);
						 next_header = (ui8*)header + header->GetSize())
					{
						if (((MarginPtr)next_header)->IsAllocated())
						{
							break;
						}
						else
						{
							MarginPtr claim_header = (MarginPtr)next_header;

							NodePtr claim_node = (NodePtr)((ui8*)claim_header + __detail::MARGIN_ALIGNED_SIZE);
							_tree.Remove(claim_node);

							header->SetSize(header->GetSize() + claim_header->GetSize());
							MarginPtr claim_footer =
								(MarginPtr)((ui8*)header + header->GetSize() - __detail::MARGIN_ALIGNED_SIZE);
							claim_footer->Value = header->Value;
						}
					}
				}

				Block dealloc_block{header, header->GetSize()};
				StowFreeBlock(dealloc_block);
				deallocated = true;
			}

			return deallocated;
		}

		bl DeallocateAll() override
		{
			Lock lock(_mutex);
			Init();
			return true;
		}
	};
} // namespace np::memory

#endif /* NP_ENGINE_RED_BLACK_TREE_ALLOCATOR_HPP */
