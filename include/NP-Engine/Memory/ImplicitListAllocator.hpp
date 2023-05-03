//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_IMPLICIT_ALLOCATOR_HPP
#define NP_ENGINE_IMPLICIT_ALLOCATOR_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "BookkeepingAllocator.hpp"
#include "Margin.hpp"
#include "MemoryFunctions.hpp"

namespace np::mem
{
	class ImplicitListAllocator : public BookkeepingAllocator
	{
	private:
		using Margin = __detail::Margin;

		constexpr static siz MARGIN_SIZE = __detail::MARGIN_SIZE;

	public:
		constexpr static siz BOOKKEEPING_SIZE = (MARGIN_SIZE * 2);
		constexpr static siz OVERHEAD_SIZE = BOOKKEEPING_SIZE;

	private:
		mutex _mutex;

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
				*footer = *header;
				initialized = true;
			}

			return initialized;
		}

		Margin* FindAllocationHeader(siz size, bl true_best_false_first)
		{
			Margin* header = nullptr;
			Margin* it;

			if (true_best_false_first)
			{
				siz diff, min_diff = SIZ_MAX;
				for (it = (Margin*)_block.Begin(); Contains(it); it = (Margin*)((ui8*)it + it->GetSize()))
				{
					if (!it->IsAllocated() && it->GetSize() >= size)
					{
						diff = it->GetSize() - size;
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
				for (it = (Margin*)_block.Begin(); Contains(it) && !header; it = (Margin*)((ui8*)it + it->GetSize()))
					if (!it->IsAllocated() && it->GetSize() >= size)
						header = it;
			}

			return header;
		}

		Block InternalAllocate(siz size, bl true_best_false_first)
		{
			Block block{}, split{};
			size = CalcAlignedSize(size) + BOOKKEEPING_SIZE;
			// size is always > OVERHEAD_SIZE

			Margin* header = FindAllocationHeader(size, true_best_false_first);
			if (header)
			{
				block = {header, header->GetSize()};

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

				header->SetIsAllocated(true);
				Margin* footer = (Margin*)((ui8*)block.End() - MARGIN_SIZE);
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
				Margin* header = (Margin*)((ui8*)ptr - MARGIN_SIZE);
				Margin* footer = (Margin*)((ui8*)header + header->GetSize() - MARGIN_SIZE);
				Margin *prev_footer, *next_header;

				// claim previous blocks
				for (prev_footer = (Margin*)((ui8*)header - MARGIN_SIZE); Contains(prev_footer) && !prev_footer->IsAllocated();
					 prev_footer = (Margin*)((ui8*)header - MARGIN_SIZE))
					header = (Margin*)((ui8*)prev_footer + MARGIN_SIZE - prev_footer->GetSize());

				// claim next blocks
				for (next_header = (Margin*)((ui8*)footer + MARGIN_SIZE); Contains(next_header) && !next_header->IsAllocated();
					 next_header = (Margin*)((ui8*)footer + MARGIN_SIZE))
					footer = (Margin*)((ui8*)next_header - MARGIN_SIZE + next_header->GetSize());

				Block block{header, (ui8*)footer - (ui8*)header + MARGIN_SIZE};
				bl success = InitFreeBlock(block);
				NP_ENGINE_ASSERT(success, "we should have a successful deallocation here");
				deallocated = true;
			}

			return deallocated;
		}

		void Init()
		{
			if (!InitFreeBlock(_block))
				_block.Zeroize();
		}

	public:
		ImplicitListAllocator(Block block): BookkeepingAllocator(block)
		{
			Init();
		}

		ImplicitListAllocator(siz size): BookkeepingAllocator(size)
		{
			Init();
		}

		Block Allocate(siz size) override
		{
			return AllocateBest(size);
		}

		Block AllocateFirst(siz size)
		{
			scoped_lock l(_mutex);
			return InternalAllocate(size, false);
		}

		Block AllocateBest(siz size)
		{
			scoped_lock l(_mutex);
			return InternalAllocate(size, true);
		}

		Block ExtractBlock(void* block_ptr) override
		{
			Block block;
			if (Contains(block_ptr))
			{
				Margin* block_header = (Margin*)((ui8*)block_ptr - MARGIN_SIZE);
				block = {block_ptr, block_header->GetSize() - BOOKKEEPING_SIZE};
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
			scoped_lock l(_mutex);
			return InternalDeallocate(ptr);
		}

		bl DeallocateAll() override
		{
			scoped_lock l(_mutex);
			Init();
			return true;
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_IMPLICIT_ALLOCATOR_HPP */
