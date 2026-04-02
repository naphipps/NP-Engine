//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/1/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_EXPLICIT_SEGREGATED_LIST_ALLOCATOR_HPP
#define NP_ENGINE_MEM_EXPLICIT_SEGREGATED_LIST_ALLOCATOR_HPP

#include <type_traits>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "ExplicitListAllocator.hpp"
#include "BookkeepingAllocator.hpp"
#include "Margin.hpp"
#include "MemoryFunctions.hpp"

namespace np::mem
{
	class explicit_segregated_list_allocator : public bookkeeping_allocator
	{
	protected:
		using node = __detail::explicit_list_allocator_node;
		using margin = __detail::margin;

		constexpr static siz MARGIN_SIZE = calc_aligned_size(sizeof(margin), DEFAULT_ALIGNMENT);
		constexpr static siz NODE_SIZE = calc_aligned_size(sizeof(node), DEFAULT_ALIGNMENT);
		constexpr static siz BOOKKEEPING_SIZE = MARGIN_SIZE * 2;
		constexpr static siz OVERHEAD_SIZE = BOOKKEEPING_SIZE + NODE_SIZE;

		mutex _mutex;
		node* _root_1_16; // roots are from aligned sizes A to B
		node* _root_17_32;
		node* _root_33_64;
		node* _root_65_;

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

		virtual node** get_root(siz size) const
		{
			node** root = nullptr;
			if (_root_1_16 && size <= 16)
				root = ::std::remove_const_t<node**>(address_of(_root_1_16));
			else if (_root_17_32 && size <= 32)
				root = ::std::remove_const_t<node**>(address_of(_root_17_32));
			else if (_root_33_64 && size <= 64)
				root = ::std::remove_const_t<node**>(address_of(_root_33_64));
			else
				root = ::std::remove_const_t<node**>(address_of(_root_65_));
			return root;
		}

		virtual node** get_root_matching(node* ptr) const
		{
			node** root = nullptr;
			if (_root_1_16 == ptr)
				root = ::std::remove_const_t<node**>(address_of(_root_1_16));
			else if (_root_17_32 == ptr)
				root = ::std::remove_const_t<node**>(address_of(_root_17_32));
			else if (_root_33_64 == ptr)
				root = ::std::remove_const_t<node**>(address_of(_root_33_64));
			else if (_root_65_ == ptr)
				root = ::std::remove_const_t<node**>(address_of(_root_65_));
			return root;
		}

		static void detach_node(node* n, node** root)
		{
			if (n->next)
				n->next->prev = n->prev;

			(n->prev ? n->prev->next : *root) = n->next;
		}

		static void attach_node(node* n, node** root)
		{
			if (n->next)
				n->next->prev = n;

			(n->prev ? n->prev->next : *root) = n;
		}

		virtual bl init_free_block(block& b)
		{
			bl initialized = false;
			if (b.size >= OVERHEAD_SIZE)
			{
				block header_block{b.begin(), MARGIN_SIZE };
				block footer_block{(ui8*)b.end() - MARGIN_SIZE, MARGIN_SIZE };
				margin* header = mem::construct<margin>(header_block, b.size, false);
				margin* footer = mem::construct<margin>(footer_block, *header);

				block node_block{header_block.end(), NODE_SIZE};
				node* n = mem::construct<node>(node_block);
				initialized = n;
			}
			return initialized;
		}

		virtual void stow_free_block(block& b)
		{
			bl init_success = init_free_block(b);
			NP_ENGINE_ASSERT(init_success, "our init here should always succeed");

			node* n = (node*)((ui8*)b.begin() + MARGIN_SIZE);
			node** root = get_root(b.size);
			node* insert = *root;

			while (contains(insert) && insert->next && insert->next < n->next)
				insert = insert->next;

			n->next = insert ? insert->next : nullptr;
			n->prev = insert;
			attach_node(n, root);
		}

		/*
			alignment is assumed pre-considered within given size
		*/
		virtual margin* find_allocation_header(siz size, bl true_best_false_first)
		{
			margin *node_header, *header = nullptr;
			node *n, *root = *get_root(size);

			if (true_best_false_first)
			{
				siz diff, min_diff = SIZ_MAX;
				for (n = root; contains(n); n = n->next)
				{
					node_header = (margin*)((ui8*)n - MARGIN_SIZE);
					if (!node_header->is_allocated() && node_header->get_size() >= size)
					{
						diff = node_header->get_size() - size;
						if (diff == 0)
						{
							header = node_header;
							break;
						}
						else if (diff < min_diff)
						{
							header = node_header;
							min_diff = diff;
						}
					}
				}
			}
			else
			{
				for (n = root; contains(n) && !header; n = n->next)
				{
					node_header = (margin*)((ui8*)n - MARGIN_SIZE);
					if (!node_header->is_allocated() && node_header->get_size() >= size)
						header = node_header;
				}
			}

			return header;
		}

		virtual block internal_allocate(siz size, siz alignment, bl true_best_false_first)
		{
			const siz aligned_size = calc_aligned_size(size, alignment);

			block b{}, split{};
			siz contiguous_size = calc_aligned_size(size, alignment) + BOOKKEEPING_SIZE;
			if (contiguous_size < OVERHEAD_SIZE)
				contiguous_size = OVERHEAD_SIZE;

			margin* header = find_allocation_header(contiguous_size, true_best_false_first);
			if (header)
			{
				b = {header, header->get_size()};
				detach_node((node*)((ui8*)header + MARGIN_SIZE), get_root(contiguous_size));

				// can we split?
				if (b.size - contiguous_size >= OVERHEAD_SIZE)
				{
					split = {(ui8*)b.begin() + contiguous_size, b.size - contiguous_size };
					b.size -= split.size;

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
				{
					bl success = internal_deallocate((ui8*)split.begin() + MARGIN_SIZE);
					NP_ENGINE_ASSERT(success, "InternalDeallocate here must succeed");
				}

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

		virtual bl internal_deallocate(void* ptr)
		{
			bl deallocated = false;
			if (contains(ptr))
			{
				margin* header = extract_header_ptr(ptr);
				if (header)
				{
					header->set_is_allocated(false);

					margin* prev_footer, * next_header, * claim_header, * claim_footer;
					node* claim_node;

					// claim previous blocks
					for (prev_footer = (margin*)((ui8*)header - MARGIN_SIZE); contains(prev_footer);
						prev_footer = (margin*)((ui8*)header - MARGIN_SIZE))
					{
						if (!prev_footer->is_allocated())
						{
							claim_header = (margin*)((ui8*)prev_footer + MARGIN_SIZE - prev_footer->get_size());
							claim_node = (node*)((ui8*)claim_header + MARGIN_SIZE);
							detach_node(claim_node, get_root(claim_header->get_size()));

							claim_header->set_size(claim_header->get_size() + header->get_size());
							claim_footer = (margin*)((ui8*)claim_header + claim_header->get_size() - MARGIN_SIZE);
							*claim_footer = *claim_header;
							header = claim_header;
							continue;
						}
						break;
					}

					// claim next blocks
					for (next_header = (margin*)((ui8*)header + header->get_size()); contains(next_header);
						next_header = (margin*)((ui8*)header + header->get_size()))
					{
						if (!next_header->is_allocated())
						{
							claim_header = next_header;
							claim_node = (node*)((ui8*)claim_header + MARGIN_SIZE);
							detach_node(claim_node, get_root(claim_header->get_size()));

							header->set_size(header->get_size() + claim_header->get_size());
							claim_footer = (margin*)((ui8*)header + header->get_size() - MARGIN_SIZE);
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

		virtual void init()
		{
			_root_1_16 = nullptr;
			_root_17_32 = nullptr;
			_root_33_64 = nullptr;
			_root_65_ = nullptr;

			if (init_free_block(_block))
				*get_root(_block.size) = static_cast<node*>(static_cast<void*>(static_cast<ui8*>(_block.begin()) + MARGIN_SIZE));
		}

	public:
		static siz get_overhead_size()
		{
			return OVERHEAD_SIZE;
		}

		explicit_segregated_list_allocator(block b):
			bookkeeping_allocator(b),
			_root_1_16(nullptr),
			_root_17_32(nullptr),
			_root_33_64(nullptr),
			_root_65_(nullptr)
		{
			init();
		}

		explicit_segregated_list_allocator(siz size, siz aligment):
			bookkeeping_allocator(size, aligment),
			_root_1_16(nullptr),
			_root_17_32(nullptr),
			_root_33_64(nullptr),
			_root_65_(nullptr)
		{
			init();
		}

		virtual ~explicit_segregated_list_allocator() = default;

		virtual block allocate(siz size, siz alignment) override
		{
			return allocate_best(size, alignment);
		}

		virtual block allocate_best(siz size, siz alignment)
		{
			scoped_lock l(_mutex);
			return internal_allocate(size, alignment, true);
		}

		virtual block allocate_first(siz size, siz alignment)
		{
			scoped_lock l(_mutex);
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
			scoped_lock l(_mutex);
			return internal_deallocate(ptr);
		}

		virtual bl deallocate_all() override
		{
			scoped_lock l(_mutex);
			init();
			return true;
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_MEM_EXPLICIT_SEGREGATED_LIST_ALLOCATOR_HPP */
