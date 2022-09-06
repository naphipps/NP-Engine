//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/13/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_SIZED_ALLOCATOR_HPP
#define NP_ENGINE_SIZED_ALLOCATOR_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "CAllocator.hpp"
#include "Block.hpp"

namespace np::mem
{
	class SizedAllocator : public CAllocator
	{
	protected:
		Block _block;
		const bl _owns_block;

	public:
		SizedAllocator(Block block): CAllocator(), _block(block), _owns_block(false) {}

		SizedAllocator(siz size): CAllocator(), _block(CAllocator::Allocate(size)), _owns_block(true) {}

		virtual ~SizedAllocator()
		{
			if (_owns_block)
				CAllocator::Deallocate(_block);
		}

		virtual bl Contains(const Block& block) const override
		{
			return _block.Contains(block);
		}

		virtual bl Contains(const void* ptr) const override
		{
			return _block.Contains(ptr);
		}

		virtual siz Size() const
		{
			return _block.size;
		}

		virtual bl OwnsBlock() const
		{
			return _owns_block;
		}

		virtual void Zeroize()
		{
			_block.Zeroize();
		}

		virtual Block Allocate(siz size) override = 0;

		virtual Block Reallocate(Block& old_block, siz new_size) override = 0;

		virtual Block Reallocate(void* old_ptr, siz new_size) override = 0;

		virtual bl Deallocate(Block& block) override = 0;

		virtual bl Deallocate(void* ptr) override = 0;

		virtual bl DeallocateAll() = 0;
	};

	constexpr static siz KILOBYTE_SIZE = 1000;
	constexpr static siz MEGABYTE_SIZE = 1000000;
	constexpr static siz GIGABYTE_SIZE = 1000000000;
} // namespace np::mem

#endif /* NP_ENGINE_SIZED_ALLOCATOR_HPP */
