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
		class RedBlackTreeAllocatorNode
		{
		private:
			NP_ENGINE_STATIC_ASSERT(ALIGNMENT >= 4,
									"This implementation requires ALIGNMENT >= 4 because we use those last two bits.");

		public:
			constexpr static siz RED_TREE_NODE_RANK = 0;
			constexpr static siz BLACK_TREE_NODE_RANK = BIT(0);
			constexpr static siz LIST_NODE_RANK = BIT(1);

		private:
			siz _parent_prev_rank = RED_TREE_NODE_RANK; // rank is in the unused bits due to alignment
			RedBlackTreeAllocatorNode* _left = nullptr;
			RedBlackTreeAllocatorNode* _right = nullptr;
			RedBlackTreeAllocatorNode* _next = nullptr;

		public:
			RedBlackTreeAllocatorNode* GetParent() const
			{
				return (RedBlackTreeAllocatorNode*)(_parent_prev_rank & ~ALIGNMENT_MINUS_ONE);
			}

			RedBlackTreeAllocatorNode* GetLeft() const
			{
				return _left;
			}

			RedBlackTreeAllocatorNode* GetRight() const
			{
				return _right;
			}

			RedBlackTreeAllocatorNode* GetPrev() const
			{
				return GetRank() == LIST_NODE_RANK ? GetParent() : nullptr;
			}

			RedBlackTreeAllocatorNode* GetNext() const
			{
				return _next;
			}

			siz GetRank() const
			{
				return _parent_prev_rank & ALIGNMENT_MINUS_ONE;
			}

			void SetParent(RedBlackTreeAllocatorNode* parent)
			{
				_parent_prev_rank = ((siz)parent & ~ALIGNMENT_MINUS_ONE) | GetRank();
			}

			void SetLeft(RedBlackTreeAllocatorNode* left)
			{
				_left = left;
			}

			void SetRight(RedBlackTreeAllocatorNode* right)
			{
				_right = right;
			}

			void SetPrev(RedBlackTreeAllocatorNode* prev)
			{
				if (GetRank() == LIST_NODE_RANK)
					SetParent(prev);
			}

			void SetNext(RedBlackTreeAllocatorNode* next)
			{
				_next = next;
			}

			void SetRank(siz rank)
			{
				_parent_prev_rank = (siz)GetParent() | (rank & ALIGNMENT_MINUS_ONE);
			}
		};

		/*
			rules:
				- (1) root property: root is black
				- (2) external property: every external node is black -- these are the nullptr leaves
				- (3) red property: children of a red node are black
				- (4) depth property: all external nodes have the same black-depth
					- black-depth: the count of black nodes along the simple path from the root to the given node
				- (5) same sized node are put on top of each other in a dbl linked list
		*/
		class RedBlackTreeAllocatorTree
		{
		public:
			using Node = RedBlackTreeAllocatorNode;

		private:
			Node* _root;

			siz GetSize(Node* node) const
			{
				return node ? ((Margin*)((ui8*)node - MARGIN_SIZE))->GetSize() : 0;
			}

			void InsertInList(Node* prev, Node* node)
			{
				node->SetRank(Node::LIST_NODE_RANK);

				// prev stays in-place, so properties of the top node (tree properties) are maintained
				node->SetPrev(prev);
				node->SetNext(prev->GetNext());
				prev->SetNext(node);

				if (node->GetNext())
					node->GetNext()->SetPrev(node);
			}

			void RemoveInList(Node* node)
			{
				if (node->GetNext())
					node->GetNext()->SetPrev(node->GetPrev());

				if (node->GetPrev())
					node->GetPrev()->SetNext(node->GetNext());
				else if (node->GetNext())
				{
					// no prev means our node was on the top, so we need to pass our tree properties to next
					Node* next = node->GetNext();
					next->SetRank(node->GetRank());
					next->SetLeft(node->GetLeft());
					next->SetRight(node->GetRight());
					next->SetParent(node->GetParent());

					if (next->GetLeft())
						next->GetLeft()->SetParent(next);

					if (next->GetRight())
						next->GetRight()->SetParent(next);

					if (next->GetParent())
					{
						if (next->GetParent()->GetLeft() == node)
							next->GetParent()->SetLeft(next);
						else
							next->GetParent()->SetRight(next);
					}
					else
					{
						_root = next;
					}
				}
			}

			Node* GetMin(Node* node)
			{
				if (node)
					while (node->GetLeft())
						node = node->GetLeft();

				return node;
			}

			Node* FindInsertParent(Node* node)
			{
				Node* it = _root;
				Node* parent = nullptr;
				siz node_size = GetSize(node);

				while (it)
				{
					siz it_size = GetSize(it);
					i32 cmp = node_size < it_size ? -1 : node_size - it_size;

					parent = it;
					if (cmp < 0)
						it = it->GetLeft();
					else if (cmp > 0)
						it = it->GetRight();
					else
						break;
				}

				return parent;
			}

			void SwapRanks(Node* a, Node* b)
			{
				siz rank = a->GetRank();
				a->SetRank(b->GetRank());
				b->SetRank(rank);
			}

			void RotateLeft(Node* parent)
			{
				Node* right = parent->GetRight();
				Node* grand_parent = parent->GetParent();

				if (grand_parent)
				{
					if (grand_parent->GetLeft() == parent)
						grand_parent->SetLeft(right);
					else
						grand_parent->SetRight(right);
				}
				else
				{
					_root = right;
				}

				parent->SetRight(right->GetLeft());
				if (parent->GetRight())
					parent->GetRight()->SetParent(parent);

				right->SetParent(grand_parent);
				right->SetLeft(parent);
				parent->SetParent(right);
			}

			void RotateRight(Node* parent)
			{
				Node* left = parent->GetLeft();
				Node* grand_parent = parent->GetParent();

				if (grand_parent)
				{
					if (grand_parent->GetLeft() == parent)
						grand_parent->SetLeft(left);
					else
						grand_parent->SetRight(left);
				}
				else
				{
					_root = left;
				}

				parent->SetLeft(left->GetRight());
				if (parent->GetLeft())
					parent->GetLeft()->SetParent(parent);

				left->SetParent(grand_parent);
				left->SetRight(parent);
				parent->SetParent(left);
			}

			void InternalInsert(Node* parent, Node* node)
			{
				node->SetRank(Node::RED_TREE_NODE_RANK);
				if (!parent)
				{
					_root = node;
					InternalInsertCleanup(node);
				}
				else
				{
					siz parent_size = GetSize(parent);
					siz node_size = GetSize(node);
					i32 cmp = node_size < parent_size ? -1 : node_size - parent_size;

					if (cmp < 0)
					{
						NP_ENGINE_ASSERT(!parent->GetLeft(), "parent->left must be nullptr here");
						parent->SetLeft(node);
						node->SetParent(parent);
						InternalInsertCleanup(node);
					}
					else if (cmp > 0)
					{
						NP_ENGINE_ASSERT(!parent->GetRight(), "parent->left must be nullptr here");
						parent->SetRight(node);
						node->SetParent(parent);
						InternalInsertCleanup(node);
					}
					else
					{
						InsertInList(parent, node);
					}
				}
			}

			void InternalInsertCleanup(Node* node)
			{
				if (node->GetParent())
				{
					if (node->GetParent()->GetRank() == Node::RED_TREE_NODE_RANK && node->GetParent()->GetParent())
					{
						Node* parent = node->GetParent();
						Node* grand_parent = parent->GetParent();
						Node* uncle = grand_parent->GetLeft() == parent ? grand_parent->GetRight() : grand_parent->GetLeft();

						siz grand_parent_size = GetSize(grand_parent);
						siz parent_size = GetSize(parent);
						siz node_size = GetSize(node);

						if (!uncle || uncle->GetRank() == Node::BLACK_TREE_NODE_RANK)
						{
							if (parent_size < grand_parent_size)
							{
								// parent is grand_parent->left
								if (parent_size < node_size)
								{
									SwapRanks(node, parent);
									RotateLeft(parent);

									node = parent;
									parent = node->GetParent();
								}

								SwapRanks(parent, grand_parent);
								RotateRight(grand_parent);
							}
							else
							{
								// parent is grand_parent->right
								if (node_size < parent_size)
								{
									SwapRanks(node, parent);
									RotateRight(parent);

									node = parent;
									parent = node->GetParent();
								}

								SwapRanks(parent, grand_parent);
								RotateLeft(grand_parent);
							}
						}
						else
						{
							parent->SetRank(Node::BLACK_TREE_NODE_RANK);
							uncle->SetRank(Node::BLACK_TREE_NODE_RANK);
							grand_parent->SetRank(Node::RED_TREE_NODE_RANK);

							if (!grand_parent->GetParent() || grand_parent->GetParent()->GetRank() == Node::RED_TREE_NODE_RANK)
								InternalInsertCleanup(grand_parent);
						}
					}
				}
				else
				{
					node->SetRank(Node::BLACK_TREE_NODE_RANK);
				}
			}

			void InternalRemove(Node* node)
			{
				if (node->GetLeft())
				{
					if (node->GetRight())
					{
						Node* replacer = GetMin(node->GetRight());
						InternalRemove(replacer);

						replacer->SetRank(node->GetRank());
						replacer->SetLeft(node->GetLeft());
						replacer->SetRight(node->GetRight());
						replacer->SetParent(node->GetParent());

						if (replacer->GetLeft())
							replacer->GetLeft()->SetParent(replacer);

						if (replacer->GetRight())
							replacer->GetRight()->SetParent(replacer);

						if (replacer->GetParent())
						{
							if (replacer->GetParent()->GetLeft() == node)
								replacer->GetParent()->SetLeft(replacer);
							else
								replacer->GetParent()->SetRight(replacer);
						}
						else
						{
							_root = replacer;
						}
					}
					else
					{
						node->GetLeft()->SetParent(node->GetParent());

						if (node->GetParent())
						{
							if (node->GetParent()->GetLeft() == node)
								node->GetParent()->SetLeft(node->GetLeft());
							else
								node->GetParent()->SetRight(node->GetLeft());
						}
						else
						{
							_root = node->GetLeft();
						}

						if (node->GetRank() == Node::BLACK_TREE_NODE_RANK)
							node->GetLeft()->SetRank(Node::BLACK_TREE_NODE_RANK);
					}
				}
				else if (node->GetRight())
				{
					node->GetRight()->SetParent(node->GetParent());

					if (node->GetParent())
					{
						if (node->GetParent()->GetLeft() == node)
							node->GetParent()->SetLeft(node->GetRight());
						else
							node->GetParent()->SetRight(node->GetRight());
					}
					else
					{
						_root = node->GetRight();
					}

					if (node->GetRank() == Node::BLACK_TREE_NODE_RANK)
						node->GetRight()->SetRank(Node::BLACK_TREE_NODE_RANK);
				}
				else
				{
					if (_root == node)
					{
						_root = nullptr;
					}
					else
					{
						if (node->GetRank() == Node::BLACK_TREE_NODE_RANK)
							InternalRemoveCleanup(node);

						if (node->GetParent()->GetLeft() == node)
							node->GetParent()->SetLeft(nullptr);
						else
							node->GetParent()->SetRight(nullptr);
					}
				}

				if (_root)
				{
					_root->SetParent(nullptr);
					_root->SetRank(Node::BLACK_TREE_NODE_RANK);
				}
			}

			void InternalRemoveCleanup(Node* node)
			{
				// node has no children
				Node* sibling = nullptr;
				if (node->GetParent()->GetLeft() == node)
					sibling = node->GetParent()->GetRight();
				else
					sibling = node->GetParent()->GetLeft();

				if (sibling->GetRank() == Node::BLACK_TREE_NODE_RANK)
				{
					if ((sibling->GetLeft() && sibling->GetLeft()->GetRank() == Node::RED_TREE_NODE_RANK) ||
						(sibling->GetRight() && sibling->GetRight()->GetRank() == Node::RED_TREE_NODE_RANK))
						InternalRemoveCleanupCase1(node); // sibling has a red child
					else
						InternalRemoveCleanupCase2(node); // sibling has no red children
				}
				else
				{
					InternalRemoveCleanupCase3(node);
				}
			}

			void InternalRemoveCleanupCase1(Node* node)
			{
				// sibling is black and has a red child
				Node* sibling = nullptr;
				if (node->GetParent()->GetLeft() == node)
					sibling = node->GetParent()->GetRight();
				else
					sibling = node->GetParent()->GetLeft();

				i32 old_parent_rank = sibling->GetParent()->GetRank();
				if (GetSize(sibling) < GetSize(node))
				{
					// sibling is node->parent->left
					if (sibling->GetRight() && sibling->GetRight()->GetRank() == Node::RED_TREE_NODE_RANK)
					{
						// sibling has red child on right
						// we need to prep sibling for a rotate right
						SwapRanks(sibling, sibling->GetRight());
						RotateLeft(sibling);
						sibling = sibling->GetParent();
					}

					RotateRight(sibling->GetParent());
				}
				else
				{
					// sibling is node->parent->right
					if (sibling->GetLeft() && sibling->GetLeft()->GetRank() == Node::RED_TREE_NODE_RANK)
					{
						// sibling has red child on left
						// we need to prep sibling for a rotate left
						SwapRanks(sibling, sibling->GetLeft());
						RotateRight(sibling);
						sibling = sibling->GetParent();
					}

					RotateLeft(sibling->GetParent());
				}

				sibling->SetRank(old_parent_rank);
				node->SetRank(Node::BLACK_TREE_NODE_RANK);

				if (sibling->GetRight())
					sibling->GetRight()->SetRank(Node::BLACK_TREE_NODE_RANK);

				if (sibling->GetLeft())
					sibling->GetLeft()->SetRank(Node::BLACK_TREE_NODE_RANK);
			}

			void InternalRemoveCleanupCase2(Node* node)
			{
				// sibling has no red children
				// sibling may have any number of black children
				Node* sibling = nullptr;
				if (node->GetParent()->GetLeft() == node)
					sibling = node->GetParent()->GetRight();
				else
					sibling = node->GetParent()->GetLeft();

				node->SetRank(Node::BLACK_TREE_NODE_RANK);
				sibling->SetRank(Node::RED_TREE_NODE_RANK);

				if (node->GetParent()->GetRank() == Node::RED_TREE_NODE_RANK)
					node->GetParent()->SetRank(Node::BLACK_TREE_NODE_RANK);
				else if (_root != node->GetParent())
					InternalRemoveCleanup(node->GetParent());
			}

			void InternalRemoveCleanupCase3(Node* node)
			{
				// sibling is red and node->parent is black
				Node* sibling = nullptr;
				if (node->GetParent()->GetLeft() == node)
					sibling = node->GetParent()->GetRight();
				else
					sibling = node->GetParent()->GetLeft();

				if (GetSize(sibling) < GetSize(node))
					RotateRight(sibling->GetParent()); // sibling is node->parent->left
				else
					RotateLeft(sibling->GetParent()); // sibling is node->parent->right

				sibling->SetRank(Node::BLACK_TREE_NODE_RANK);
				node->GetParent()->SetRank(Node::RED_TREE_NODE_RANK);
				InternalRemoveCleanup(node);
			}

		public:
			RedBlackTreeAllocatorTree(): _root(nullptr) {}

			Node* SearchBest(siz size)
			{
				Node* found = nullptr;
				siz diff, min_diff = SIZ_MAX;
				for (Node* it = _root; it; it = GetSize(it) > size ? it->GetLeft() : it->GetRight())
				{
					if (GetSize(it) >= size)
					{
						diff = GetSize(it) - size;
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
				}
				return found;
			}

			Node* SearchFirst(siz size)
			{
				Node* found = nullptr;
				for (Node* it = _root; it && !found; it = it->GetRight())
					if (GetSize(it) >= size)
						found = it;

				return found;
			}

			void Insert(Node* node)
			{
				*node = {};
				node->SetRank(Node::RED_TREE_NODE_RANK);
				InternalInsert(FindInsertParent(node), node);
			}

			void Remove(Node* node)
			{
				if (node->GetRank() == Node::LIST_NODE_RANK || node->GetNext())
					RemoveInList(node);
				else
					InternalRemove(node);
			}

			void Clear()
			{
				_root = nullptr;
			}
		};
	} // namespace __detail

	class RedBlackTreeAllocator : public BookkeepingAllocator
	{
	private:
		using Tree = __detail::RedBlackTreeAllocatorTree;
		using Node = Tree::Node;
		using Margin = __detail::Margin;

		constexpr static siz MARGIN_SIZE = __detail::MARGIN_SIZE;
		constexpr static siz NODE_SIZE = CalcAlignedSize(sizeof(Node));

	public:
		constexpr static siz BOOKKEEPING_SIZE = (MARGIN_SIZE * 2);
		constexpr static siz OVERHEAD_SIZE = BOOKKEEPING_SIZE + NODE_SIZE;

	private:
		mutex _mutex;
		Tree _tree;

		bl InitFreeBlock(Block& block)
		{
			bl initialized = false;
			if (block.size >= OVERHEAD_SIZE)
			{
				Block header_block{block.Begin(), MARGIN_SIZE};
				Block footer_block{(ui8*)block.End() - MARGIN_SIZE, MARGIN_SIZE};
				Margin* header = mem::Construct<Margin>(header_block);
				Margin* footer = mem::Construct<Margin>(footer_block);
				header->SetSize(block.size);
				header->SetIsAllocated(false);
				*footer = *header;

				Block node_block{header_block.End(), NODE_SIZE};
				Node* node = mem::Construct<Node>(node_block);
				initialized = node;
			}

			return initialized;
		}

		void StowFreeBlock(Block& block)
		{
			bl init_success = InitFreeBlock(block);
			NP_ENGINE_ASSERT(init_success, "our init here should always succeed");
			_tree.Insert((Node*)((ui8*)block.Begin() + MARGIN_SIZE));
		}

		Margin* FindAllocationHeader(siz size, bl true_best_false_first)
		{
			Node* node = true_best_false_first ? _tree.SearchBest(size) : _tree.SearchFirst(size);
			return node ? (Margin*)((ui8*)node - MARGIN_SIZE) : nullptr;
		}

		Block InternalAllocate(siz size, bl true_best_false_first)
		{
			scoped_lock l(_mutex);

			Block block{}, split{};
			size = CalcAlignedSize(size) + BOOKKEEPING_SIZE;
			if (size < OVERHEAD_SIZE)
				size = OVERHEAD_SIZE;

			Margin* header = FindAllocationHeader(size, true_best_false_first);
			if (header)
			{
				block = {header, header->GetSize()};
				_tree.Remove((Node*)((ui8*)header + MARGIN_SIZE));

				// can we split?
				if (block.size - size >= OVERHEAD_SIZE)
				{
					split = {(ui8*)block.Begin() + size, block.size - size};
					block.size = size;

					bl success = InitFreeBlock(split);
					NP_ENGINE_ASSERT(success, "InitFreeBlock must be successful here");

					success = InitFreeBlock(block);
					NP_ENGINE_ASSERT(success, "InitFreeBlock must be successful here");
				}

				header->SetIsAllocated(true);
				Margin* footer = (Margin*)((ui8*)block.End() - MARGIN_SIZE);
				*footer = *header;

				// only deallocate split after our block is considered allocated
				if (split.IsValid())
					StowFreeBlock(split);

				block.ptr = (ui8*)block.Begin() + MARGIN_SIZE;
				block.size -= BOOKKEEPING_SIZE;
			}

			return block;
		}

		bl InternalDeallocate(void* ptr)
		{
			scoped_lock l(_mutex);
			bl deallocated = false;

			if (Contains(ptr))
			{
				Margin* header = (Margin*)((ui8*)ptr - MARGIN_SIZE);
				header->SetIsAllocated(false);

				Margin *prev_footer, *next_header, *claim_footer, *claim_header;
				Node* claim_node;

				// claim previous blocks
				for (prev_footer = (Margin*)((ui8*)header - MARGIN_SIZE); Contains(prev_footer);
					 prev_footer = (Margin*)((ui8*)header - MARGIN_SIZE))
				{
					if (!prev_footer->IsAllocated())
					{
						claim_header = (Margin*)((ui8*)prev_footer + MARGIN_SIZE - prev_footer->GetSize());
						claim_node = (Node*)((ui8*)claim_header + MARGIN_SIZE);
						_tree.Remove(claim_node);

						claim_header->SetSize(claim_header->GetSize() + header->GetSize());
						claim_footer = (Margin*)((ui8*)claim_header + claim_header->GetSize() - MARGIN_SIZE);
						*claim_footer = *claim_header;
						header = claim_header;
						continue;
					}
					break;
				}

				// claim next blocks
				for (next_header = (Margin*)((ui8*)header + header->GetSize()); Contains(next_header);
					 next_header = (Margin*)((ui8*)header + header->GetSize()))
				{
					if (!next_header->IsAllocated())
					{
						claim_header = next_header;
						claim_node = (Node*)((ui8*)claim_header + MARGIN_SIZE);
						_tree.Remove(claim_node);

						header->SetSize(header->GetSize() + claim_header->GetSize());
						claim_footer = (Margin*)((ui8*)header + header->GetSize() - MARGIN_SIZE);
						*claim_footer = *header;
						continue;
					}
					break;
				}

				Block block{header, header->GetSize()};
				StowFreeBlock(block);
				deallocated = true;
			}

			return deallocated;
		}

		void Init()
		{
			scoped_lock l(_mutex);
			_tree.Clear();

			if (InitFreeBlock(_block))
			{
				Margin* header = (Margin*)_block.Begin();
				Margin* footer = (Margin*)((ui8*)_block.End() - MARGIN_SIZE);
				header->SetSize(_block.size);
				header->SetIsAllocated(false);
				*footer = *header;

				_tree.Insert((Node*)((ui8*)header + MARGIN_SIZE));
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
			return InternalAllocate(size, true);
		}

		Block AllocateFirst(siz size)
		{
			return InternalAllocate(size, false);
		}

		Block ExtractBlock(void* block_ptr) override
		{
			Block block;
			if (Contains(block_ptr))
			{
				Margin* header = (Margin*)((ui8*)block_ptr - MARGIN_SIZE);
				block = {block_ptr, header->GetSize() - BOOKKEEPING_SIZE};
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
			return InternalDeallocate(ptr);
		}

		bl DeallocateAll() override
		{
			Init();
			return true;
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_RED_BLACK_TREE_ALLOCATOR_HPP */
