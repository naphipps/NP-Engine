//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/10/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_RED_BLACK_TREE_ALLOCATOR_HPP
#define NP_ENGINE_MEM_RED_BLACK_TREE_ALLOCATOR_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "Alignment.hpp"
#include "BookkeepingAllocator.hpp"
#include "Margin.hpp"
#include "MemoryFunctions.hpp"

namespace np::mem
{
	namespace __detail
	{
		class red_black_tree_allocator_node
		{
		protected:
			NP_ENGINE_STATIC_ASSERT(DEFAULT_ALIGNMENT_BIT >= 2,
				"This implementation requires DEFAULT_ALIGNMENT_BIT >= 2 because we use those last two bits for the node's mark.");

			constexpr static siz MARK_MASK = DEFAULT_ALIGNMENT_MASK;
			constexpr static siz NODE_MASK = ~MARK_MASK;
			constexpr static siz RED_TREE_NODE_MARK = 0;
			constexpr static siz BLACK_TREE_NODE_MARK = BIT(0);
			constexpr static siz LIST_NODE_MARK = BIT(1);

			red_black_tree_allocator_node* _parent = nullptr; //used as prev (when is list node), and mark is embedded within
			red_black_tree_allocator_node* _left = nullptr;
			red_black_tree_allocator_node* _right = nullptr;
			red_black_tree_allocator_node* _next = nullptr;

			static red_black_tree_allocator_node* extract_node_ptr(red_black_tree_allocator_node* ptr_)
			{
				union {
					red_black_tree_allocator_node* ptr = nullptr;
					siz value;
				};

				ptr = ptr_;
				value &= NODE_MASK;
				return ptr;
			}

			static red_black_tree_allocator_node* embed_node_ptr(red_black_tree_allocator_node* dst, red_black_tree_allocator_node* src)
			{
				union {
					red_black_tree_allocator_node* ptr = nullptr;
					siz value;
				};

				ptr = extract_node_ptr(src);
				value |= extract_mark(dst);
				return ptr;
			}

			static siz extract_mark(red_black_tree_allocator_node* ptr_)
			{
				union {
					red_black_tree_allocator_node* ptr = nullptr;
					siz value;
				};

				ptr = ptr_;
				return value & MARK_MASK;
			}

			static red_black_tree_allocator_node* embed_mark(red_black_tree_allocator_node* dst, siz src)
			{
				union {
					red_black_tree_allocator_node* ptr = nullptr;
					siz value;
				};

				ptr = extract_node_ptr(dst);
				value |= src & MARK_MASK;
				return ptr;
			}

		public:

			red_black_tree_allocator_node* get_parent() const
			{
				return extract_node_ptr(_parent);
			}

			red_black_tree_allocator_node* get_left() const
			{
				return _left;
			}

			red_black_tree_allocator_node* get_right() const
			{
				return _right;
			}

			red_black_tree_allocator_node* get_next() const
			{
				return _next;
			}

			red_black_tree_allocator_node* get_prev() const
			{
				return is_list_node() ? get_parent() : nullptr;
			}

			void set_parent(red_black_tree_allocator_node* n)
			{
				_parent = embed_node_ptr(_parent, n);
			}

			void set_left(red_black_tree_allocator_node* n)
			{
				_left = n;
			}

			void set_right(red_black_tree_allocator_node* n)
			{
				_right = n;
			}

			void set_next(red_black_tree_allocator_node* n)
			{
				_next = n;
			}

			void set_prev(red_black_tree_allocator_node* n)
			{
				if (is_list_node())
					set_parent(n);
			}

			siz get_mark() const
			{
				return extract_mark(_parent);
			}

			bl is_red_tree_node() const
			{
				return get_mark() == RED_TREE_NODE_MARK;
			}

			bl is_black_tree_node() const
			{
				return get_mark() == BLACK_TREE_NODE_MARK;
			}

			bl is_list_node() const
			{
				return get_mark() == LIST_NODE_MARK;
			}

			void set_mark(siz mark)
			{
				_parent = embed_mark(_parent, mark);
			}

			void mark_as_red_tree_node()
			{
				set_mark(RED_TREE_NODE_MARK);
			}

			void mark_as_blank_tree_node()
			{
				set_mark(BLACK_TREE_NODE_MARK);
			}

			void mark_as_list_node()
			{
				set_mark(LIST_NODE_MARK);
			}

			void swap_marks(red_black_tree_allocator_node* other)
			{
				const siz mark = get_mark();
				set_mark(other->get_mark());
				other->set_mark(mark);
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
		class red_black_tree_allocator_tree
		{
		protected:
			using node = red_black_tree_allocator_node;

			constexpr static siz MARGIN_SIZE = calc_aligned_size(sizeof(margin), DEFAULT_ALIGNMENT);

			node* _root;

			siz get_size(node* n) const
			{
				return n ? ((margin*)((ui8*)n - MARGIN_SIZE))->get_size() : 0;
			}

			void insert_into_list(node* prev, node* n)
			{
				n->mark_as_list_node();

				// prev stays in-place, so properties of the top node (tree properties) are maintained
				n->set_prev(prev);
				n->set_next(prev->get_next());
				prev->set_next(n);

				if (n->get_next())
					n->get_next()->set_prev(n);
			}

			void remove_from_list(node* n)
			{
				if (n->get_next())
					n->get_next()->set_prev(n->get_prev());

				if (n->get_prev())
				{
					n->get_prev()->set_next(n->get_next());
				}
				else if (n->get_next())
				{
					// no prev means our node was on the top, so we need to pass our tree properties to next
					node* next = n->get_next();
					next->set_mark(n->get_mark());
					next->set_left(n->get_left());
					next->set_right(n->get_right());
					next->set_parent(n->get_parent());

					if (next->get_left())
						next->get_left()->set_parent(next);

					if (next->get_right())
						next->get_right()->set_parent(next);

					if (next->get_parent())
					{
						if (next->get_parent()->get_left() == n)
							next->get_parent()->set_left(next);
						else
							next->get_parent()->set_right(next);
					}
					else
					{
						_root = next;
					}
				}
			}

			node* get_min(node* n)
			{
				if (n)
					while (n->get_left())
						n = n->get_left();
				return n;
			}

			node* find_insert_parent(node* n)
			{
				node* parent = nullptr;
				const siz node_size = get_size(n);

				for (node* it = _root; it;)
				{
					siz it_size = get_size(it);
					i32 cmp = node_size < it_size ? -1 : node_size - it_size;

					parent = it;
					if (cmp < 0)
						it = it->get_left();
					else if (cmp > 0)
						it = it->get_right();
					else
						break;
				}

				return parent;
			}

			void rotate_left(node* parent)
			{
				node* right = parent->get_right();
				node* grand_parent = parent->get_parent();

				if (grand_parent)
				{
					if (grand_parent->get_left() == parent)
						grand_parent->set_left(right);
					else
						grand_parent->set_right(right);
				}
				else
				{
					_root = right;
				}

				parent->set_right(right->get_left());
				if (parent->get_right())
					parent->get_right()->set_parent(parent);

				right->set_parent(grand_parent);
				right->set_left(parent);
				parent->set_parent(right);
			}

			void rotate_right(node* parent)
			{
				node* left = parent->get_left();
				node* grand_parent = parent->get_parent();

				if (grand_parent)
				{
					if (grand_parent->get_left() == parent)
						grand_parent->set_left(left);
					else
						grand_parent->set_right(left);
				}
				else
				{
					_root = left;
				}

				parent->set_left(left->get_right());
				if (parent->get_left())
					parent->get_left()->set_parent(parent);

				left->set_parent(grand_parent);
				left->set_right(parent);
				parent->set_parent(left);
			}

			siz _internal_insert_count = 0;

			void internal_insert(node* parent, node* n)
			{
				_internal_insert_count++;

				n->mark_as_red_tree_node();
				if (!parent)
				{
					_root = n;
					internal_insert_cleanup(n);
				}
				else
				{
					siz parent_size = get_size(parent);
					siz node_size = get_size(n);
					i32 cmp = node_size < parent_size ? -1 : node_size - parent_size;

					if (cmp < 0)
					{
						NP_ENGINE_ASSERT(!parent->get_left(), "parent->get_left() must be nullptr here");
						parent->set_left(n);
						n->set_parent(parent);
						internal_insert_cleanup(n);
					}
					else if (cmp > 0)
					{
						NP_ENGINE_ASSERT(!parent->get_right(), "parent->get_left() must be nullptr here");
						parent->set_right(n);
						n->set_parent(parent);
						internal_insert_cleanup(n);
					}
					else
					{
						insert_into_list(parent, n);
					}
				}
			}

			void internal_insert_cleanup(node* n)
			{
				if (n->get_parent())
				{
					if (n->get_parent()->is_red_tree_node() && n->get_parent()->get_parent())
					{
						node* parent = n->get_parent();
						node* grand_parent = parent->get_parent();
						node* uncle = grand_parent->get_left() == parent ? grand_parent->get_right() : grand_parent->get_left();

						siz grand_parent_size = get_size(grand_parent);
						siz parent_size = get_size(parent);
						siz node_size = get_size(n);

						if (!uncle || uncle->is_black_tree_node())
						{
							if (parent_size < grand_parent_size)
							{
								// parent is grand_parent->get_left()
								if (parent_size < node_size)
								{
									n->swap_marks(parent);
									rotate_left(parent);

									n = parent;
									parent = n->get_parent();
								}

								parent->swap_marks(grand_parent);
								rotate_right(grand_parent);
							}
							else
							{
								// parent is grand_parent->get_right()
								if (node_size < parent_size)
								{
									n->swap_marks(parent);
									rotate_right(parent);

									n = parent;
									parent = n->get_parent();
								}

								parent->swap_marks(grand_parent);
								rotate_left(grand_parent);
							}
						}
						else
						{
							parent->mark_as_blank_tree_node();
							uncle->mark_as_blank_tree_node();
							grand_parent->mark_as_red_tree_node();

							if (!grand_parent->get_parent() || grand_parent->get_parent()->is_red_tree_node())
								internal_insert_cleanup(grand_parent);
						}
					}
				}
				else
				{
					n->mark_as_blank_tree_node();
				}
			}

			siz _internal_remove_count = 0;

			void internal_remove(node* n)
			{
				_internal_remove_count++;

				if (n->get_left())
				{
					if (n->get_right())
					{
						node* replacer = get_min(n->get_right());
						internal_remove(replacer);

						replacer->set_mark(n->get_mark());
						replacer->set_left(n->get_left());
						replacer->set_right(n->get_right());
						replacer->set_parent(n->get_parent());

						if (replacer->get_left())
							replacer->get_left()->set_parent(replacer);

						if (replacer->get_right())
							replacer->get_right()->set_parent(replacer);

						if (replacer->get_parent())
						{
							if (replacer->get_parent()->get_left() == n)
								replacer->get_parent()->set_left(replacer);
							else
								replacer->get_parent()->set_right(replacer);
						}
						else
						{
							_root = replacer;
						}
					}
					else
					{
						n->get_left()->set_parent(n->get_parent());

						if (n->get_parent())
						{
							if (n->get_parent()->get_left() == n)
								n->get_parent()->set_left(n->get_left());
							else
								n->get_parent()->set_right(n->get_left());
						}
						else
						{
							_root = n->get_left();
						}

						if (n->is_black_tree_node())
							n->get_left()->mark_as_blank_tree_node();
					}
				}
				else if (n->get_right())
				{
					n->get_right()->set_parent(n->get_parent());

					if (n->get_parent())
					{
						if (n->get_parent()->get_left() == n)
							n->get_parent()->set_left(n->get_right());
						else
							n->get_parent()->set_right(n->get_right());
					}
					else
					{
						_root = n->get_right();
					}

					if (n->is_black_tree_node())
						n->get_right()->mark_as_blank_tree_node();
				}
				else
				{
					if (_root == n)
					{
						_root = nullptr;
					}
					else
					{
						if (n->is_black_tree_node())
							internal_remove_cleanup(n);

						if (n->get_parent()->get_left() == n)
							n->get_parent()->set_left(nullptr);
						else
							n->get_parent()->set_right(nullptr);
					}
				}

				if (_root)
				{
					_root->set_parent(nullptr);
					_root->mark_as_blank_tree_node();
				}
			}

			void internal_remove_cleanup(node* n)
			{
				// node has no children
				node* sibling = n->get_parent()->get_left() == n ? n->get_parent()->get_right() : n->get_parent()->get_left();

				if (sibling->get_parent() != n->get_parent())
				{
					//something's wrong
					i32 x = 0;
					x++;
				}

				if (sibling->is_black_tree_node())
				{
					if ((sibling->get_left() && sibling->get_left()->is_red_tree_node()) ||
						(sibling->get_right() && sibling->get_right()->is_red_tree_node()))
						internal_remove_cleanup_case_1(n); // sibling has a red child
					else
						internal_remove_cleanup_case_2(n); // sibling has no red children
				}
				else
				{
					internal_remove_cleanup_case_3(n);
				}
			}

			void internal_remove_cleanup_case_1(node* n)
			{
				// sibling is black and has a red child
				node* sibling = n->get_parent()->get_left() == n ? n->get_parent()->get_right() : n->get_parent()->get_left();

				const siz old_parent_rank = sibling->get_parent()->get_mark();
				if (get_size(sibling) < get_size(n))
				{
					// sibling is node->get_parent()->get_left()
					if (sibling->get_right() && sibling->get_right()->is_red_tree_node())
					{
						// sibling has red child on right
						// we need to prep sibling for a rotate right
						sibling->swap_marks(sibling->get_right());
						rotate_left(sibling);
						sibling = sibling->get_parent();
					}

					rotate_right(sibling->get_parent());
				}
				else
				{
					// sibling is node->get_parent()->get_right()
					if (sibling->get_left() && sibling->get_left()->is_red_tree_node())
					{
						// sibling has red child on left
						// we need to prep sibling for a rotate left
						sibling->swap_marks(sibling->get_left());
						rotate_right(sibling);
						sibling = sibling->get_parent();
					}

					rotate_left(sibling->get_parent());
				}

				sibling->set_mark(old_parent_rank);
				n->mark_as_blank_tree_node();

				if (sibling->get_left())
					sibling->get_left()->mark_as_blank_tree_node();

				if (sibling->get_right())
					sibling->get_right()->mark_as_blank_tree_node();
			}

			void internal_remove_cleanup_case_2(node* n)
			{
				// sibling has no red children
				// sibling may have any number of black children
				node* sibling = n->get_parent()->get_left() == n ? n->get_parent()->get_right() : n->get_parent()->get_left();

				n->mark_as_blank_tree_node();
				sibling->mark_as_red_tree_node();

				if (n->get_parent()->is_red_tree_node())
					n->get_parent()->mark_as_blank_tree_node();
				else if (_root != n->get_parent())
					internal_remove_cleanup(n->get_parent());
			}

			void internal_remove_cleanup_case_3(node* n)
			{
				// sibling is red and node->get_parent() is black
				node* sibling = n->get_parent()->get_left() == n ? n->get_parent()->get_right() : n->get_parent()->get_left();

				if (get_size(sibling) < get_size(n))
					rotate_right(sibling->get_parent()); // sibling is node->get_parent()->get_left()
				else
					rotate_left(sibling->get_parent()); // sibling is node->get_parent()->get_right()

				sibling->mark_as_blank_tree_node();
				n->get_parent()->mark_as_red_tree_node();
				internal_remove_cleanup(n);
			}

		public:
			red_black_tree_allocator_tree(): _root(nullptr) {}

			node* search_best(siz size)
			{
				node* found = nullptr;
				siz diff, min_diff = SIZ_MAX;
				for (node* it = _root; it; it = get_size(it) > size ? it->get_left() : it->get_right())
				{
					if (get_size(it) >= size)
					{
						diff = get_size(it) - size;
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

			node* search_first(siz size)
			{
				node* found = nullptr;
				for (node* it = _root; it && !found; it = it->get_right())
					if (get_size(it) >= size)
						found = it;

				return found;
			}

			void insert(node* n)
			{
				*n = {};
				n->mark_as_red_tree_node();
				internal_insert(find_insert_parent(n), n);
			}

			void remove(node* n)
			{
				if (n->is_list_node() || n->get_next())
					remove_from_list(n);
				else
					internal_remove(n);
			}

			void clear()
			{
				_root = nullptr;
			}
		};
	} // namespace __detail

	class red_black_tree_allocator : public bookkeeping_allocator
	{
	protected:
		using tree = __detail::red_black_tree_allocator_tree;
		using node = __detail::red_black_tree_allocator_node;
		using margin = __detail::margin;

		constexpr static siz MARGIN_SIZE = calc_aligned_size(sizeof(margin), DEFAULT_ALIGNMENT);
		constexpr static siz NODE_SIZE = calc_aligned_size(sizeof(node), DEFAULT_ALIGNMENT);
		constexpr static siz BOOKKEEPING_SIZE = MARGIN_SIZE * 2;
		constexpr static siz OVERHEAD_SIZE = BOOKKEEPING_SIZE + NODE_SIZE;

		mutex _mutex;
		tree _tree;

		virtual margin* extract_header_ptr(void* ptr)
		{
			//we take advantage of the convenience that sizeof(margin) must equal DEFAULT_ALIGNMENT

			margin* header = nullptr;
			if (contains(ptr) && is_aligned(ptr, DEFAULT_ALIGNMENT))
			{
				ui8* it = static_cast<ui8*>(ptr) - MARGIN_SIZE;

				for (siz i = 0; i < NP_ENGINE_MEM_ATTEMPT_MARGIN_EXTRACTION_COUNT && is_zeroized(it, MARGIN_SIZE); i++)
					it -= DEFAULT_ALIGNMENT;

				//check if we found the header
				header = !is_zeroized(it, MARGIN_SIZE) ? static_cast<margin*>(static_cast<void*>(it)) : nullptr;
			}
			return header;
		}

		bl init_free_block(block& b)
		{
			bl initialized = false;
			if (b.size >= OVERHEAD_SIZE)
			{
				block header_block{b.begin(), MARGIN_SIZE};
				block footer_block{(ui8*)b.end() - MARGIN_SIZE, MARGIN_SIZE};
				margin* header = mem::construct<margin>(header_block, b.size, false);
				margin* footer = mem::construct<margin>(footer_block, *header);

				block node_block{header_block.end(), NODE_SIZE};
				node* n = mem::construct<node>(node_block);
				initialized = n;
			}

			return initialized;
		}

		void stow_free_block(block& b)
		{
			bl init_success = init_free_block(b);
			NP_ENGINE_ASSERT(init_success, "our init here should always succeed");
			_tree.insert((node*)((ui8*)b.begin() + MARGIN_SIZE));
		}

		margin* find_allocation_header(siz size, bl true_best_false_first)
		{
			node* node = true_best_false_first ? _tree.search_best(size) : _tree.search_first(size);
			return node ? (margin*)((ui8*)node - MARGIN_SIZE) : nullptr;
		}

		block internal_allocate(siz size, siz alignment, bl true_best_false_first)
		{
			scoped_lock l(_mutex);
			const siz aligned_size = calc_aligned_size(size, alignment);

			block b{}, split{};
			siz contiguous_size = aligned_size + BOOKKEEPING_SIZE;
			if (contiguous_size < OVERHEAD_SIZE)
				contiguous_size = OVERHEAD_SIZE;

			margin* header = find_allocation_header(contiguous_size, true_best_false_first);
			if (header)
			{
				b = {header, header->get_size()};
				_tree.remove((node*)((ui8*)header + MARGIN_SIZE));

				// can we split?
				if (b.size - contiguous_size >= OVERHEAD_SIZE)
				{
					split = {(ui8*)b.begin() + contiguous_size, b.size - contiguous_size };
					b.size = contiguous_size;

					bl success = init_free_block(split);
					NP_ENGINE_ASSERT(success, "init_free_block must be successful here");

					success = init_free_block(b);
					NP_ENGINE_ASSERT(success, "init_free_block must be successful here");
				}

				header->set_is_allocated(true);
				margin* footer = (margin*)((ui8*)b.end() - MARGIN_SIZE);
				*footer = *header;

				// only deallocate split after our block is considered allocated
				if (split.is_valid())
					stow_free_block(split);

				b = { static_cast<ui8*>(b.begin()) + MARGIN_SIZE, b.size - BOOKKEEPING_SIZE };
			}

			if (b.is_valid())
			{
				::std::pair<block, block> split = b.get_split_on_alignment(alignment);
				split.first.zeroize(); //this is how we find our margin on deallocation
				b = split.second;
			}

			return b;
		}

		bl internal_deallocate(void* ptr)
		{
			scoped_lock l(_mutex);
			bl deallocated = false;

			if (contains(ptr))
			{
				margin* header = extract_header_ptr(ptr);
				if (header)
				{
					header->set_is_allocated(false);

					// claim previous blocks
					for (margin* prev_footer = (margin*)((ui8*)header - MARGIN_SIZE); contains(prev_footer);
						prev_footer = (margin*)((ui8*)header - MARGIN_SIZE))
					{
						if (!prev_footer->is_allocated())
						{
							margin* claim_header = (margin*)((ui8*)prev_footer + MARGIN_SIZE - prev_footer->get_size());
							node* claim_node = (node*)((ui8*)claim_header + MARGIN_SIZE);
							_tree.remove(claim_node);

							claim_header->set_size(claim_header->get_size() + header->get_size());
							margin* claim_footer = (margin*)((ui8*)claim_header + claim_header->get_size() - MARGIN_SIZE);
							*claim_footer = *claim_header;
							header = claim_header;
							continue;
						}
						break;
					}

					// claim next blocks
					for (margin* next_header = (margin*)((ui8*)header + header->get_size()); contains(next_header);
						next_header = (margin*)((ui8*)header + header->get_size()))
					{
						if (!next_header->is_allocated())
						{
							margin* claim_header = next_header;
							node* claim_node = (node*)((ui8*)claim_header + MARGIN_SIZE);
							_tree.remove(claim_node);

							header->set_size(header->get_size() + claim_header->get_size());
							margin* claim_footer = (margin*)((ui8*)header + header->get_size() - MARGIN_SIZE);
							*claim_footer = *header;
							continue;
						}
						break;
					}

					block b{ header, header->get_size() };
					stow_free_block(b);
					deallocated = true;
				}
			}

			return deallocated;
		}

		void init()
		{
			scoped_lock l(_mutex);
			_tree.clear();

			if (init_free_block(_block))
			{
				margin* header = (margin*)_block.begin();
				margin* footer = (margin*)((ui8*)_block.end() - MARGIN_SIZE);
				header->set_size(_block.size);
				header->set_is_allocated(false);
				*footer = *header;

				_tree.insert((node*)((ui8*)header + MARGIN_SIZE));
			}
		}

	public:
		static siz get_overhead_size()
		{
			return OVERHEAD_SIZE;
		}

		red_black_tree_allocator(block b): bookkeeping_allocator(b)
		{
			init();
		}

		red_black_tree_allocator(siz size, siz alignment): bookkeeping_allocator(size, alignment)
		{
			init();
		}

		virtual ~red_black_tree_allocator() = default;

		virtual block allocate(siz size, siz alignment) override
		{
			return allocate_best(size, alignment);
		}

		virtual block allocate_best(siz size, siz alignment)
		{
			return internal_allocate(size, alignment, true);
		}

		virtual block allocate_first(siz size, siz alignment)
		{
			return internal_allocate(size, alignment, false);
		}

		virtual block extract_allocated_block(void* ptr) override
		{
			block allocated{};
			margin* header = extract_header_ptr(ptr);
			if (header) //ptr is aligned to DEFAULT_ALIGNMENT and _block contains it
			{
				block b{ static_cast<ui8*>(static_cast<void*>(header)) + MARGIN_SIZE, header->get_size() - BOOKKEEPING_SIZE };
				block offset = b.get_offset_block(ptr);

				if (offset.end() == ptr || (offset.empty() && b.begin() == ptr)) //confirm that we found the correct offset
					allocated = b.get_remaining_block(offset);
			}
			return allocated;
		}

		virtual block reallocate(block& b, siz size, siz alignment) override
		{
			return reallocate_best(b, size, alignment);
		}

		virtual block reallocate(void* ptr, siz size, siz alignment) override
		{
			return reallocate_best(ptr, size, alignment);
		}

		virtual block reallocate_best(block& b_, siz size, siz alignment)
		{
			block b = allocate_best(size, alignment);
			if (contains(b_))
			{
				const siz byte_count = b.size < b_.size ? b.size : b_.size;
				copy_bytes(b.begin(), b_.begin(), byte_count);
				deallocate(b_);
				b_.invalidate();
			}
			return b;
		}

		virtual block reallocate_best(void* ptr, siz size, siz alignment)
		{
			block b = extract_allocated_block(ptr);
			return reallocate_best(b, size, alignment);
		}

		virtual block reallocate_first(block& b_, siz size, siz alignment)
		{
			block b = allocate_first(size, alignment);
			if (contains(b_))
			{
				const siz byte_count = b.size < b_.size ? b.size : b_.size;
				copy_bytes(b.begin(), b_.begin(), byte_count);
				deallocate(b_);
				b_.invalidate();
			}
			return b;
		}

		virtual block reallocate_first(void* ptr, siz size, siz alignment)
		{
			block b = extract_allocated_block(ptr);
			return reallocate_first(b, size, alignment);
		}

		virtual bl deallocate(block& b) override
		{
			bl deallocated = deallocate(b.ptr);
			if (deallocated)
				b.invalidate();
			return deallocated;
		}

		virtual bl deallocate(void* ptr) override
		{
			return internal_deallocate(ptr);
		}

		virtual bl deallocate_all() override
		{
			init();
			return true;
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_MEM_RED_BLACK_TREE_ALLOCATOR_HPP */
