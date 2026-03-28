//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/13/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_BLOCKED_ALLOCATOR_HPP
#define NP_ENGINE_MEM_BLOCKED_ALLOCATOR_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Callocator.hpp"
#include "Block.hpp"

namespace np::mem
{
	class blocked_allocator : public c_allocator
	{
	protected:
		block _block;
		const bl _owns_block;

	public:
		blocked_allocator(block b): c_allocator(), _block(b), _owns_block(false) {}

		/*
			alignment is the alignment for the block we will allocate for this allocator to use
		*/
		blocked_allocator(siz size, siz alignment): c_allocator(), _block(c_allocator::allocate(size, alignment)), _owns_block(true) {}

		virtual ~blocked_allocator()
		{
			if (_owns_block)
				c_allocator::deallocate(_block);
		}

		virtual bl contains(const block& b) override
		{
			return _block.contains(b);
		}

		virtual bl contains(const void* ptr) override
		{
			return _block.contains(ptr);
		}

		virtual siz size() const
		{
			return _block.size;
		}

		virtual bl owns_block() const
		{
			return _owns_block;
		}

		virtual block allocate(siz size, siz alignment) override = 0;

		virtual block reallocate(block& b, siz size, siz alignment) override = 0;

		virtual block reallocate(void* ptr, siz size, siz alignment) override = 0;

		virtual bl deallocate(block& b) override = 0;

		virtual bl deallocate(void* ptr) override = 0;

		virtual bl deallocate_all() = 0;
	};
} // namespace np::mem

#endif /* NP_ENGINE_MEM_BLOCKED_ALLOCATOR_HPP */
