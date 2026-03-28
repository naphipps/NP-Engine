//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_IMPLICIT_ALLOCATOR_HPP
#define NP_ENGINE_MEM_IMPLICIT_ALLOCATOR_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "BookkeepingAllocator.hpp"
#include "Margin.hpp"
#include "MemoryFunctions.hpp"

namespace np::mem
{
	class implicit_list_allocator : public bookkeeping_allocator
	{
	protected:
		using margin = __detail::margin;

		constexpr static siz MARGIN_SIZE = calc_aligned_size(sizeof(margin), DEFAULT_ALIGNMENT);
		constexpr static siz BOOKKEEPING_SIZE = (MARGIN_SIZE * 2);
		constexpr static siz OVERHEAD_SIZE = BOOKKEEPING_SIZE;

		mutex _mutex;

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

		virtual bl init_free_block(block& b)
		{
			bl initialized = false;
			if (b.size >= OVERHEAD_SIZE)
			{
				block header_block{b.begin(), MARGIN_SIZE};
				block footer_block{(ui8*)b.end() - MARGIN_SIZE, MARGIN_SIZE};
				margin* header = mem::construct<margin>(header_block, b.size, false);
				margin* footer = mem::construct<margin>(footer_block, *header);
				initialized = true;
			}
			return initialized;
		}

		/*
			alignment is assumed pre-considered within given size
		*/
		virtual margin* find_allocation_header(siz size, bl true_best_false_first)
		{
			margin* header = nullptr;
			margin* it;

			if (true_best_false_first)
			{
				siz diff, min_diff = SIZ_MAX;
				for (it = (margin*)_block.begin(); contains(it); it = (margin*)((ui8*)it + it->get_size()))
				{
					if (!it->is_allocated() && it->get_size() >= size)
					{
						diff = it->get_size() - size;
						if (diff == 0)
						{
							header = it;
							break;
						}
						else if (diff < min_diff)
						{
							header = it;
							min_diff = diff;
						}
					}
				}
			}
			else
			{
				for (it = (margin*)_block.begin(); contains(it) && !header; it = (margin*)((ui8*)it + it->get_size()))
					if (!it->is_allocated() && it->get_size() >= size)
						header = it;
			}

			return header;
		}

		virtual block internal_allocate(siz size, siz alignment, bl true_best_false_first)
		{
			block b{}, split{};
			siz aligned_size = calc_aligned_size(size, alignment) + BOOKKEEPING_SIZE;
			// aligned_size is always > OVERHEAD_SIZE

			margin* header = find_allocation_header(aligned_size, true_best_false_first);
			if (header)
			{
				b = {header, header->get_size()};

				// can we split?
				if (b.size - aligned_size >= OVERHEAD_SIZE)
				{
					split = {(ui8*)b.begin() + aligned_size, b.size - aligned_size };
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
					NP_ENGINE_ASSERT(success, "internal_deallocate here must succeed");
				}

				b = { (ui8*)b.begin() + MARGIN_SIZE , b.size - BOOKKEEPING_SIZE };
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
				margin* header = (margin*)((ui8*)ptr - MARGIN_SIZE);
				margin* footer = (margin*)((ui8*)header + header->get_size() - MARGIN_SIZE);
				margin *prev_footer, *next_header;

				// claim previous blocks
				for (prev_footer = (margin*)((ui8*)header - MARGIN_SIZE); contains(prev_footer) && !prev_footer->is_allocated();
					 prev_footer = (margin*)((ui8*)header - MARGIN_SIZE))
					header = (margin*)((ui8*)prev_footer + MARGIN_SIZE - prev_footer->get_size());

				// claim next blocks
				for (next_header = (margin*)((ui8*)footer + MARGIN_SIZE); contains(next_header) && !next_header->is_allocated();
					 next_header = (margin*)((ui8*)footer + MARGIN_SIZE))
					footer = (margin*)((ui8*)next_header - MARGIN_SIZE + next_header->get_size());

				block b{header, (ui8*)footer - (ui8*)header + MARGIN_SIZE};
				bl success = init_free_block(b);
				NP_ENGINE_ASSERT(success, "we should have a successful deallocation here");
				deallocated = true;
			}

			return deallocated;
		}

		virtual void init()
		{
			init_free_block(_block);
		}

	public:
		static siz get_overhead_size()
		{
			return OVERHEAD_SIZE;
		}

		implicit_list_allocator(block b): bookkeeping_allocator(b)
		{
			init();
		}

		implicit_list_allocator(siz size, siz aligment): bookkeeping_allocator(size, aligment)
		{
			init();
		}

		virtual ~implicit_list_allocator() = default;

		virtual block allocate(siz size, siz alignment) override
		{
			return allocate_best(size, alignment);
		}

		virtual block allocate_first(siz size, siz alignment)
		{
			scoped_lock l(_mutex);
			return internal_allocate(size, alignment, false);
		}

		virtual block allocate_best(siz size, siz alignment)
		{
			scoped_lock l(_mutex);
			return internal_allocate(size, alignment, true);
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

#endif /* NP_ENGINE_MEM_IMPLICIT_ALLOCATOR_HPP */
