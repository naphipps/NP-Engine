//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/10/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RED_BLACK_TREE_ALLOCATOR_HPP
#define NP_ENGINE_RED_BLACK_TREE_ALLOCATOR_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "BookkeepingAllocator.hpp"
#include "Margin.hpp"
#include "MemoryFunctions.hpp"

namespace np::mem
{
	namespace __detail
	{
		struct RedBlackTreeAllocatorNode
		{
			RedBlackTreeAllocatorNode* Left = nullptr;
			RedBlackTreeAllocatorNode* Right = nullptr;
			RedBlackTreeAllocatorNode* Parent = nullptr;
			bl IsRed = false;

			siz GetSize() const
			{
				// TODO: I think we can figure out something better than this -- improve our tree too
				return ((MarginPtr)((ui8*)this - MARGIN_SIZE))->Size;
			}
		};

		/*
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
				for (; min->Left != &_nil; min = min->Left) {}
				return min;
			}

			/*
				transplants node b to a
			*/
			void Transplant(NodePtr a, NodePtr b)
			{
				Node* parent = a->Parent;
				if (parent == nullptr)
					_root = b;
				else if (a == parent->Left)
					parent->Left = b;
				else
					parent->Right = b;

				b->Parent = parent;
			}

			void RotateCCW(NodePtr node)
			{
				NodePtr parent = node->Parent;
				NodePtr right = node->Right;

				node->Right = right->Left;
				if (right->Left != &_nil)
					right->Left->Parent = node;

				right->Parent = parent;

				if (parent == nullptr)
					_root = right;
				else if (parent->Left == node)
					parent->Left = right;
				else
					parent->Right = right;

				right->Left = node;
				node->Parent = right;
			}

			void RotateCW(NodePtr node)
			{
				NodePtr parent = node->Parent;
				NodePtr left = node->Left;

				node->Left = left->Right;
				if (left->Right != &_nil)
					left->Right->Parent = node;

				left->Parent = parent;

				if (parent == nullptr)
					_root = left;
				else if (parent->Left == node)
					parent->Left = left;
				else
					parent->Right = left;

				left->Right = node;
				node->Parent = left;
			}

			void InsertCleanup(NodePtr node)
			{
				while (node->Parent != nullptr && node->Parent->IsRed)
				{
					NodePtr y = nullptr;
					if (node->Parent == node->Parent->Parent->Left)
					{
						y = node->Parent->Parent->Right;

						if (y->IsRed)
						{
							node->Parent->IsRed = false;
							y->IsRed = false;
							node->Parent->Parent->IsRed = true;
							node = node->Parent->Parent;
						}
						else
						{
							if (node == node->Parent->Right)
							{
								node = node->Parent;
								RotateCCW(node);
							}

							node->Parent->IsRed = false;
							node->Parent->Parent->IsRed = true;
							RotateCW(node->Parent->Parent);
						}
					}
					else
					{
						y = node->Parent->Parent->Left;

						if (y->IsRed)
						{
							node->Parent->IsRed = false;
							y->IsRed = false;
							node->Parent->Parent->IsRed = true;
							node = node->Parent->Parent;
						}
						else
						{
							if (node == node->Parent->Left)
							{
								node = node->Parent;
								RotateCW(node);
							}

							node->Parent->IsRed = false;
							node->Parent->Parent->IsRed = true;
							RotateCCW(node->Parent->Parent);
						}
					}
				}

				_root->IsRed = false;
			}

			void RemoveCleanup(NodePtr node)
			{
				while (node != _root && !node->IsRed)
				{
					NodePtr temp = nullptr;
					if (node->Parent->Left == node)
					{
						temp = node->Parent->Right;
						if (temp->IsRed)
						{
							temp->IsRed = false;
							node->Parent->IsRed = true;
							RotateCCW(node->Parent);
							temp = node->Parent->Right;
						}
						if ((temp->Right == &_nil || !temp->Right->IsRed) && (temp->Left == &_nil || !temp->Left->IsRed))
						{
							temp->IsRed = true;
							node = node->Parent;
						}
						else
						{
							if (temp->Right == &_nil || !temp->Right->IsRed)
							{
								temp->Left->IsRed = false;
								temp->IsRed = true;
								RotateCW(temp);
								temp = node->Parent->Right;
							}

							if (!node->Parent->IsRed)
								temp->IsRed = false;
							else
								temp->IsRed = true;

							node->Parent->IsRed = false;
							temp->Right->IsRed = false;
							RotateCCW(node->Parent);
							node = _root;
						}
					}
					else
					{
						temp = node->Parent->Left;
						if (temp->IsRed)
						{
							temp->IsRed = false;
							node->Parent->IsRed = true;
							RotateCW(node->Parent);
							temp = node->Parent->Left;
						}
						if ((temp->Right == &_nil || !temp->Right->IsRed) && (temp->Left == &_nil || !temp->Left->IsRed))
						{
							temp->IsRed = true;
							node = node->Parent;
						}
						else
						{
							if (temp->Left == &_nil || !temp->Left->IsRed)
							{
								temp->Right->IsRed = false;
								temp->IsRed = true;
								RotateCCW(temp);
								temp = node->Parent->Left;
							}

							if (!node->Parent->IsRed)
								temp->IsRed = false;
							else
								temp->IsRed = true;

							node->Parent->IsRed = false;
							temp->Left->IsRed = false;
							RotateCW(node->Parent);
							node = _root;
						}
					}
				}
				node->IsRed = false;
			}

		public:
			RedBlackTreeAllocatorTree(): _root(nullptr)
			{
				Clear();
			}

			NodePtr SearchBest(siz size)
			{
				NodePtr found = nullptr;
				siz diff, min_diff = SIZ_MAX;
				for (NodePtr it = _root; it != &_nil;)
				{
					if (it->GetSize() >= size)
					{
						diff = it->GetSize() - size;
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
					}

					if (it->GetSize() > size)
						it = it->Left;
					else
						it = it->Right;
				}
				return found;
			}

			NodePtr SearchFirst(siz size)
			{
				NodePtr found = nullptr;
				for (NodePtr it = _root; it != &_nil && !found; it = it->Right)
					if (it->GetSize() >= size)
						found = it;

				return found;
			}

			void Insert(NodePtr node)
			{
				NodePtr parent = nullptr;
				if (node != _root)
					for (NodePtr it = _root; it != &_nil; it = node->GetSize() < it->GetSize() ? it->Left : it->Right)
						parent = it;

				node->Parent = parent;

				if (parent == nullptr)
					_root = node;
				else if (node->GetSize() < parent->GetSize())
					parent->Left = node;
				else
					parent->Right = node;

				node->Left = &_nil;
				node->Right = &_nil;
				node->IsRed = true;

				InsertCleanup(node);
			}

			void Remove(NodePtr node)
			{
				NodePtr successor = nullptr;
				bl successor_is_black = !node->IsRed;

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
					successor_is_black = !min->IsRed;
					successor = min->Right;

					if (min->Parent == node)
					{
						successor->Parent = min;
					}
					else
					{
						Transplant(min, min->Right);
						min->Right = node->Right;
						if (min->Right != &_nil)
							min->Right->Parent = min;
					}

					Transplant(node, min);
					min->Left = node->Left;
					if (min->Left != &_nil)
						min->Left->Parent = min;

					min->IsRed = node->IsRed;
				}

				if (successor_is_black)
					RemoveCleanup(successor);
			}

			void Clear()
			{
				_nil = {};
				NP_ENGINE_ASSERT(_nil.GetSize() == 0, "our nil size must always be 0");
				_root = &_nil;
			}
		};
	} // namespace __detail

	class RedBlackTreeAllocator : public BookkeepingAllocator
	{
	private:
		using Tree = __detail::RedBlackTreeAllocatorTree;
		using Node = Tree::Node;
		using NodePtr = Tree::NodePtr;
		using Margin = __detail::Margin;
		using MarginPtr = __detail::MarginPtr;

		constexpr static siz MARGIN_SIZE = __detail::MARGIN_SIZE;
		constexpr static siz NODE_SIZE = CalcAlignedSize(sizeof(Node));

	public:
		constexpr static siz BOOKKEEPING_SIZE = (MARGIN_SIZE * 2);
		constexpr static siz OVERHEAD_SIZE = BOOKKEEPING_SIZE + NODE_SIZE;

	private:
		Mutex _mutex;
		Tree _tree;

		bl InitFreeBlock(Block& block)
		{
			bl initialized = false;
			if (block.size >= OVERHEAD_SIZE)
			{
				Block header_block{block.Begin(), MARGIN_SIZE};
				Block footer_block{(ui8*)block.End() - MARGIN_SIZE, MARGIN_SIZE};
				Construct<Margin>(header_block);
				Construct<Margin>(footer_block);
				MarginPtr header = static_cast<MarginPtr>(header_block.ptr);
				MarginPtr footer = static_cast<MarginPtr>(footer_block.ptr);
				header->Size = block.size;
				header->IsAllocated = false;
				*footer = *header;

				Block node_block{header_block.End(), NODE_SIZE};
				Construct<Node>(node_block);

				initialized = true;
			}

			return initialized;
		}

		void StowFreeBlock(Block& block)
		{
			bl init_success = InitFreeBlock(block);
			NP_ENGINE_ASSERT(init_success, "our init here should always succeed");
			_tree.Insert((NodePtr)((ui8*)block.Begin() + MARGIN_SIZE));
		}

		MarginPtr FindAllocationHeader(siz size, bl true_best_false_first)
		{
			NodePtr node = true_best_false_first ? _tree.SearchBest(size) : _tree.SearchFirst(size);
			return node ? (MarginPtr)((ui8*)node - MARGIN_SIZE) : nullptr;
		}

		Block InternalAllocate(siz size, bl true_best_false_first)
		{
			Block block{}, split{};
			size = CalcAlignedSize(size) + BOOKKEEPING_SIZE;
			if (size < OVERHEAD_SIZE)
				size = OVERHEAD_SIZE;

			MarginPtr header = FindAllocationHeader(size, true_best_false_first);
			if (header)
			{
				block = {header, header->Size};
				_tree.Remove((NodePtr)((ui8*)header + MARGIN_SIZE));

				// can we split?
				if (block.size - size >= OVERHEAD_SIZE)
				{
					split = {(ui8*)block.Begin() + size, block.size - size};
					block.size -= split.size;

					bl success = InitFreeBlock(split);
					NP_ENGINE_ASSERT(success, "InitFreeBlock must be successful here");

					success = InitFreeBlock(block);
					NP_ENGINE_ASSERT(success, "InitFreeBlock must be successful here");
				}

				header->IsAllocated = true;
				MarginPtr footer = (MarginPtr)((ui8*)block.End() - MARGIN_SIZE);
				*footer = *header;

				// only deallocate split after our block is considered allocated
				if (split.IsValid())
				{
					bl success = InternalDeallocate((ui8*)split.Begin() + MARGIN_SIZE);
					NP_ENGINE_ASSERT(success, "InternalDeallocate here must succeed");
				}

				block.ptr = (ui8*)block.Begin() + MARGIN_SIZE;
				block.size -= BOOKKEEPING_SIZE;
			}

			return block;
		}

		bl InternalDeallocate(void* ptr)
		{
			bl deallocated = false;

			if (Contains(ptr))
			{
				MarginPtr header = (MarginPtr)((ui8*)ptr - MARGIN_SIZE);
				header->IsAllocated = false;

				MarginPtr prev_footer, next_header, claim_footer, claim_header;
				NodePtr claim_node;

				// claim previous blocks
				for (prev_footer = (MarginPtr)((ui8*)header - MARGIN_SIZE); Contains(prev_footer);
					 prev_footer = (MarginPtr)((ui8*)header - MARGIN_SIZE))
				{
					if (!prev_footer->IsAllocated)
					{
						claim_header = (MarginPtr)((ui8*)prev_footer + MARGIN_SIZE - prev_footer->Size);
						claim_node = (NodePtr)((ui8*)claim_header + MARGIN_SIZE);
						_tree.Remove(claim_node);

						claim_header->Size = claim_header->Size + header->Size;
						claim_footer = (MarginPtr)((ui8*)claim_header + claim_header->Size - MARGIN_SIZE);
						*claim_footer = *claim_header;
						header = claim_header;
						continue;
					}
					break;
				}

				// claim next blocks
				for (next_header = (MarginPtr)((ui8*)header + header->Size); Contains(next_header);
					 next_header = (MarginPtr)((ui8*)header + header->Size))
				{
					if (!next_header->IsAllocated)
					{
						claim_header = next_header;
						claim_node = (NodePtr)((ui8*)claim_header + MARGIN_SIZE);
						_tree.Remove(claim_node);

						header->Size = header->Size + claim_header->Size;
						claim_footer = (MarginPtr)((ui8*)header + header->Size - MARGIN_SIZE);
						*claim_footer = *header;
						continue;
					}
					break;
				}

				Block block{header, header->Size};
				StowFreeBlock(block);
				deallocated = true;
			}

			return deallocated;
		}

		void Init()
		{
			_tree.Clear();

			if (InitFreeBlock(_block))
			{
				MarginPtr header = (MarginPtr)_block.Begin();
				MarginPtr footer = (MarginPtr)((ui8*)_block.End() - MARGIN_SIZE);
				header->Size = _block.size;
				header->IsAllocated = false;
				;
				*footer = *header;

				_tree.Insert((NodePtr)((ui8*)header + MARGIN_SIZE));
			}
			else
			{
				_block.Zeroize();
			}
		}

	public:
		RedBlackTreeAllocator(Block block): BookkeepingAllocator(block)
		{
			Init();
		}

		RedBlackTreeAllocator(siz size): BookkeepingAllocator(size)
		{
			Init();
		}

		Block Allocate(siz size) override
		{
			return AllocateBest(size);
		}

		Block AllocateBest(siz size)
		{
			Lock lock(_mutex);
			return InternalAllocate(size, true);
		}

		Block AllocateFirst(siz size)
		{
			Lock lock(_mutex);
			return InternalAllocate(size, false);
		}

		Block ExtractBlock(void* block_ptr) const override
		{
			Block block;
			if (Contains(block_ptr))
			{
				MarginPtr header = (MarginPtr)((ui8*)block_ptr - MARGIN_SIZE);
				block = {block_ptr, header->Size - BOOKKEEPING_SIZE};
			}

			return block;
		}

		Block Reallocate(Block& old_block, siz new_size) override
		{
			return ReallocateBest(old_block, new_size);
		}

		Block Reallocate(void* old_ptr, siz new_size) override
		{
			return ReallocateBest(old_ptr, new_size);
		}

		Block ReallocateBest(Block& old_block, siz new_size)
		{
			Block new_block = AllocateBest(new_size);
			if (Contains(old_block))
			{
				CopyBytes(new_block.Begin(), old_block.Begin(), old_block.size);
				Deallocate(old_block);
				old_block.Invalidate();
			}

			return new_block;
		}

		Block ReallocateBest(void* old_ptr, siz new_size)
		{
			Block old_block = ExtractBlock(old_ptr);
			return ReallocateBest(old_block, new_size);
		}

		Block ReallocateFirst(Block& old_block, siz new_size)
		{
			Block new_block = AllocateFirst(new_size);
			if (Contains(old_block))
			{
				CopyBytes(new_block.Begin(), old_block.Begin(), old_block.size);
				Deallocate(old_block);
				old_block.Invalidate();
			}

			return new_block;
		}

		Block ReallocateFirst(void* old_ptr, siz new_size)
		{
			Block old_block = ExtractBlock(old_ptr);
			return ReallocateFirst(old_block, new_size);
		}

		bl Deallocate(Block& block) override
		{
			bl deallocated = Deallocate(block.ptr);
			if (deallocated)
				block.Invalidate();

			return deallocated;
		}

		bl Deallocate(void* ptr) override
		{
			Lock lock(_mutex);
			return InternalDeallocate(ptr);
		}

		bl DeallocateAll() override
		{
			Lock lock(_mutex);
			Init();
			return true;
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_RED_BLACK_TREE_ALLOCATOR_HPP */
