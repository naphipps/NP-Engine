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
		using MarginPtr = __detail::MarginPtr;

		constexpr static siz MARGIN_SIZE = __detail::MARGIN_SIZE;

	public:
		constexpr static siz BOOKKEEPING_SIZE = (MARGIN_SIZE * 2);
		constexpr static siz OVERHEAD_SIZE = BOOKKEEPING_SIZE;

	private:
		Mutex _mutex;

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
				*footer = *header;
				initialized = true;
			}

			return initialized;
		}

		MarginPtr FindAllocationHeader(siz size, bl true_best_false_first)
		{
			MarginPtr header = nullptr;
			MarginPtr it;

			if (true_best_false_first)
			{
				siz diff, min_diff = SIZ_MAX;
				for (it = (MarginPtr)_block.Begin(); Contains(it); it = (MarginPtr)((ui8*)it + it->Size))
				{
					if (!it->IsAllocated && it->Size >= size)
					{
						diff = it->Size - size;
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
				for (it = (MarginPtr)_block.Begin(); Contains(it) && !header; it = (MarginPtr)((ui8*)it + it->Size))
					if (!it->IsAllocated && it->Size >= size)
						header = it;
			}

			return header;
		}

		Block InternalAllocate(siz size, bl true_best_false_first)
		{
			Block block{}, split{};
			size = CalcAlignedSize(size) + BOOKKEEPING_SIZE;
			// size is always > OVERHEAD_SIZE

			MarginPtr header = FindAllocationHeader(size, true_best_false_first);
			if (header)
			{
				block = {header, header->Size};

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
				MarginPtr footer = (MarginPtr)((ui8*)header + header->Size - MARGIN_SIZE);
				MarginPtr prev_footer, next_header;

				// claim previous blocks
				for (prev_footer = (MarginPtr)((ui8*)header - MARGIN_SIZE); Contains(prev_footer) && !prev_footer->IsAllocated;
					 prev_footer = (MarginPtr)((ui8*)header - MARGIN_SIZE))
					header = (MarginPtr)((ui8*)prev_footer + MARGIN_SIZE - prev_footer->Size);

				// claim next blocks
				for (next_header = (MarginPtr)((ui8*)footer + MARGIN_SIZE); Contains(next_header) && !next_header->IsAllocated;
					 next_header = (MarginPtr)((ui8*)footer + MARGIN_SIZE))
					footer = (MarginPtr)((ui8*)next_header - MARGIN_SIZE + next_header->Size);

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
			Lock lock(_mutex);
			return InternalAllocate(size, false);
		}

		Block AllocateBest(siz size)
		{
			Lock lock(_mutex);
			return InternalAllocate(size, true);
		}

		Block ExtractBlock(void* block_ptr) const override
		{
			Block block;
			if (Contains(block_ptr))
			{
				MarginPtr block_header = (MarginPtr)((ui8*)block_ptr - MARGIN_SIZE);
				block = {block_ptr, block_header->Size - BOOKKEEPING_SIZE};
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

#endif /* NP_ENGINE_IMPLICIT_ALLOCATOR_HPP */
