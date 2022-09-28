//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/10/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RED_BLACK_TREE_ALLOCATOR_HPP
#define NP_ENGINE_RED_BLACK_TREE_ALLOCATOR_HPP

#include <iostream> //TODO: remove
#include <set> //TODO: remove
#include <vector> //TODO: remove
#include <map> //TODO: remove

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "BookkeepingAllocator.hpp"
#include "Margin.hpp"
#include "MemoryFunctions.hpp"

namespace np::mem
{
	namespace __detail
	{
		class NodeT
		{
		private:
			NP_ENGINE_STATIC_ASSERT(ALIGNMENT >= 4, "This implementation requires ALIGNMENT >= 4 because we use those last two bits.");

			siz _parent_prev_rank = 0;
			NodeT* _left = nullptr;
			NodeT* _right = nullptr;
			NodeT* _next = nullptr;

		public:

			constexpr static siz RED_TREE_NODE_RANK = 0;
			constexpr static siz BLACK_TREE_NODE_RANK = BIT(0);
			constexpr static siz DOUBLE_BLACK_TREE_NODE_RANK = BIT(1);
			constexpr static siz LIST_NODE_RANK = BIT(0) | BIT(1);

			NodeT* GetParent() const
			{
				return (NodeT*)(_parent_prev_rank & ~ALIGNMENT_MINUS_ONE);
			}

			NodeT* GetLeft() const
			{
				return _left;
			}

			NodeT* GetRight() const
			{
				return _right;
			}

			NodeT* GetPrev() const
			{
				return GetRank() == LIST_NODE_RANK ? GetParent() : nullptr;
			}

			NodeT* GetNext() const
			{
				return _next;
			}

			siz GetRank() const
			{
				return _parent_prev_rank & ALIGNMENT_MINUS_ONE;
			}

			void SetParent(NodeT* parent)
			{
				_parent_prev_rank = (siz)parent & GetRank();
			}

			void SetLeft(NodeT* left)
			{
				_left = left;
			}

			void SetRight(NodeT* right)
			{
				_right = right;
			}

			void SetPrev(NodeT* prev)
			{
				if (GetRank() == LIST_NODE_RANK)
					SetParent(prev);
			}

			void SetNext(NodeT* next) 
			{
				_next = next;
			}

			void SetRank(siz rank)
			{
				_parent_prev_rank = (siz)GetParent() | (rank & ALIGNMENT_MINUS_ONE);
			}

			//TODO: go back to POD class with setters and getters
			//TODO: compress rank with parent
		};

		struct RedBlackTreeAllocatorNode
		{
			RedBlackTreeAllocatorNode* left = nullptr;
			RedBlackTreeAllocatorNode* right = nullptr;
			RedBlackTreeAllocatorNode* parent = nullptr;

			RedBlackTreeAllocatorNode* next = nullptr;
			RedBlackTreeAllocatorNode* prev = nullptr;

			i32 rank = 0;
		};

		/*
			rules:
			1- root and leaves(nil's) are black
			2- if node is red, the parent is black
			3- all simple paths from any node x to descendent leaf have the same number of black nodes
		*/
		class RedBlackTreeAllocatorTree
		{
		public:
			using Node = RedBlackTreeAllocatorNode;

		private:
			Node* _root;

			siz GetSize(Node* node) const //TODO: this could probably go back to the newer Node implementation
			{
				return node ? ((MarginPtr)((ui8*)node - MARGIN_SIZE))->Size : 0;
			}

			void InsertInList(Node* prev, Node* node)
			{
				//prev stays in-place, so properties of the top node (tree properties) are maintained
				node->prev = prev;
				node->next = prev->next;
				prev->next = node;

				if (node->next)
					node->next->prev = node;
			}

			void RemoveInList(Node* node)
			{
				//TODO: I think we can simplify and remove the two ptrs below
				Node* prev = node->prev;
				Node* next = node->next;

				if (next)
					next->prev = prev;

				if (prev)
					prev->next = next;
				else if (next)
				{
					//no prev means our node was on the top, so we need to pass our tree properties to next
					next->left = node->left;
					next->right = node->right;
					next->parent = node->parent;
					next->rank = node->rank;

					if (next->left)
						next->left->parent = next;

					if (next->right)
						next->right->parent = next;

					if (next->parent)
					{
						if (next->parent->left == node)
							next->parent->left = next;
						else
							next->parent->right = next;
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
					while (node->left)
						node = node->left;

				return node;
			}

			Node* GetMax(Node* node) //TODO: remove??
			{
				if (node)
					while (node->right)
						node = node->right;

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
						it = it->left;
					else if (cmp > 0)
						it = it->right;
					else
						break;
				}

				return parent;
			}

			void InternalInsert(Node* parent, Node* node)
			{
				if (!parent)
				{
					_root = node;
				}
				else
				{
					siz parent_size = GetSize(parent);
					siz node_size = GetSize(node);
					i32 cmp = node_size < parent_size ? -1 : node_size - parent_size;

					if (cmp < 0)
					{
						if (GetSize(parent->left) < node_size)
							node->left = parent->left;
						else
							node->right = parent->left;

						parent->left = node;
						node->parent = parent;

						if (node->left)
							node->left->parent = node;

						if (node->right)
							node->right->parent = node;

						//TODO: tree changed - call cleanup here
					}
					else if (cmp > 0)
					{
						if (GetSize(parent->right) < node_size)
							node->left = parent->right;
						else
							node->right = parent->right;

						parent->right = node;
						node->parent = parent;

						if (node->left)
							node->left->parent = node;

						if (node->right)
							node->right->parent = node;

						//TODO: tree changed - call cleanup here
					}
					else
					{
						InsertInList(parent, node);
					}
				}

				if (_root)
					_root->parent = nullptr;
			}

			void InternalInsertCleanup(Node* node) {}

			void InternalRemove(Node* node)
			{
				Node* replacer = GetMin(node->right);
				if (replacer)
				{
					//detach replacer
					if (replacer->parent->left == replacer)
					{
						replacer->parent->left = replacer->right;
						if (replacer->parent->left)
							replacer->parent->left->parent = replacer->parent;
					}
					else //replacer is node_right
					{
						node->right = replacer->right;
						if (node->right)
							node->right->parent = node;
					}

					//replace node
					replacer->left = node->left;
					replacer->right = node->right;
					replacer->parent = node->parent;

					if (replacer->left)
						replacer->left->parent = replacer;

					if (replacer->right)
						replacer->right->parent = replacer;

					if (replacer->parent)
					{
						if (replacer->parent->left == node)
							replacer->parent->left = replacer;
						else
							replacer->parent->right = replacer;
					}
					else
					{
						_root = replacer;
					}
				}
				else if (node->left)
				{
					node->left->parent = node->parent;

					if (node->left->parent)
					{
						if (node->left->parent->left == node)
							node->left->parent->left = node->left;
						else
							node->left->parent->right = node->left;
					}
					else
					{
						_root = node->left;
					}
				}
				else if (node->parent)
				{
					if (node->parent->left == node)
						node->parent->left = nullptr;
					else
						node->parent->right = nullptr;
				}
				else
				{
					_root = nullptr;
				}

				if (_root)
					_root->parent = nullptr;

				//TODO: tree changed - call cleanup here
			}

			void InternalRemoveCleanup(Node* node) {}

			void PushBlack(Node* node)
			{
				node->rank = 0;
				 
				if (node->left)
					node->left->rank = 1;

				if (node->right)
					node->right->rank = 1;
			}

			void PullBlack(Node* node)
			{
				node->rank = 1;

				if (node->left)
					node->left->rank = 0;

				if (node->right)
					node->right->rank = 0;
			}

			void SwapColors(Node* a, Node* b)
			{
				i32 rank = a->rank;
				a->rank = b->rank;
				b->rank = rank;
			}

			void RotateLeft(Node* node)
			{

			}

			void RotateRight(Node* node)
			{

			}

			void FlipLeft(Node* node)
			{
				SwapColors(node, node->right);
				RotateLeft(node);
			}

			void FlipRight(Node* node)
			{
				SwapColors(node, node->left);
				RotateRight(node);
			}

		public:
			RedBlackTreeAllocatorTree(): _root(nullptr) {}

			Node* SearchBest(siz size)
			{
				Node* found = nullptr;
				siz diff, min_diff = SIZ_MAX;
				for (Node* it = _root; it;
					it = GetSize(it) > size ? it->left : it->right)
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
				for (Node* it = _root; it && !found; it = it->right)
					if (GetSize(it) >= size)
						found = it;

				return found;
			}

			void Insert(Node* node)
			{
				//PrepFoundNodes();

				*node = {};
				InternalInsert(FindInsertParent(node), node);

				/*
				_found_nodes.emplace_back(node);
				ErrorCheck();
				*/
			}

			::std::vector<Node*> _original_found_nodes; //TODO: remove

			void Remove(Node* node)
			{
				/*
				PrepFoundNodes();
				for (auto it = _found_nodes.begin(); it != _found_nodes.end(); it++)
				{
					if (*it == node)
					{
						_found_nodes.erase(it);
						break;
					}
				}
				*/

				if (node->prev || node->next)
					RemoveInList(node);
				else
					InternalRemove(node);

				//ErrorCheck();

				*node = {};
			}

			void Clear()
			{
				_root = nullptr;
			}



			//TODO: remove methods and fields below


			Block _block;

			bl Contains(void* ptr)
			{
				return _block.Contains(ptr);
			}

			::std::vector<Node*> _found_nodes;

			void PrepFoundNodes()
			{
				_found_nodes.clear();

				FindNodes(_root);

				_original_found_nodes.clear();
				for (Node*& node : _found_nodes)
					_original_found_nodes.emplace_back(node);
			}

			void FindNodes(Node* node)
			{
				if (node)
				{
					_found_nodes.emplace_back(node);
					FindNodes(node->left);
					FindNodes(node->right);

					Node* it = node->prev;
					while (it)
					{
						_found_nodes.emplace_back(it);
						it = it->prev;
					}

					it = node->next;
					while (it)
					{
						_found_nodes.emplace_back(it);
						it = it->next;
					}
				}
			}

			bl TreeHasNode(Node* node, Node* find)
			{
				bl has = false;

				if (node)
				{
					has |= TreeHasNode(node->left, find);
					has |= TreeHasNode(node->right, find);

					Node* it = node;
					while (it)
					{
						has |= it == find;
						it = it->prev;
					}

					it = node->next;
					while (it)
					{
						has |= it == find;
						it = it->next;
					}
				}

				return has;
			}

			void ErrorCheck()
			{
				ErrorCheck(_root);

				for (Node*& node : _found_nodes)
				{
					//NP_ENGINE_ASSERT(TreeHasNode(_root, node), "tree must have all expected nodes");
					if (!TreeHasNode(_root, node))
					{
						::std::cout << "tree must contain expected nodes\n";
					}

				}
			}

			void ErrorCheck(Node* it)
			{
				if (it)
				{
					if (it->parent)
					{
						NP_ENGINE_ASSERT(it != it->parent, "it cannot be its parent");
						NP_ENGINE_ASSERT(it->parent->left == it || it->parent->right == it, "its parent does not recognize it");
					}
					else
					{
						NP_ENGINE_ASSERT(_root == it, "root should be it");
					}

					NP_ENGINE_ASSERT(Contains(it), "it is not contined");
					NP_ENGINE_ASSERT(GetSize(it) <= _block.size, "it size is out of bounds");

					ErrorCheck(it->left);
					ErrorCheck(it->right);

					ErrorListCheck(it->prev);
					ErrorListCheck(it);
					ErrorListCheck(it->next);
				}
			}

			void ErrorListCheck(Node* node)
			{
				if (node)
				{
					if (node->prev)
					{
						NP_ENGINE_ASSERT(node->prev->next == node, "prev->next must be node");
					}

					if (node->next)
					{
						NP_ENGINE_ASSERT(node->next->prev == node, "next->prev must be node");
					}
				}
			}
		};
	} // namespace __detail

	class RedBlackTreeAllocator : public BookkeepingAllocator
	{
	private:
		using Tree = __detail::RedBlackTreeAllocatorTree;
		using Node = Tree::Node;
		using NodePtr = Tree::Node*; //TODO: I think we can get rid of this
		using Margin = __detail::Margin;
		using MarginPtr = __detail::MarginPtr; //TODO: I think we can get rid of this? maybe

		constexpr static siz MARGIN_SIZE = __detail::MARGIN_SIZE;
		constexpr static siz NODE_SIZE = CalcAlignedSize(sizeof(Node));

	public:
		constexpr static siz BOOKKEEPING_SIZE = (MARGIN_SIZE * 2);
		constexpr static siz OVERHEAD_SIZE = BOOKKEEPING_SIZE + NODE_SIZE;

		siz _allocated_ = 0;
		flt _allocated_rate_ = 0.f;
		siz _allocation_count_ = 0;

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
			//TODO: I think we can move our locks to these internal methods

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
					block.size = size;

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
					StowFreeBlock(split);

				_allocated_ += header->Size;
				_allocated_rate_ = (flt)_allocated_ / (flt)_block.size;
				_allocation_count_++;

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
				_allocated_ -= header->Size;
				_allocated_rate_ = (flt)_allocated_ / (flt)_block.size;
				_allocation_count_--;

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
			_tree._block = _block;

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
