//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/1/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_SEGEREGATED_ALLOCATOR_HPP
#define NP_ENGINE_SEGEREGATED_ALLOCATOR_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "Allocator.hpp"
#include "MemoryFunctions.hpp"

namespace np::mem
{
	class SegregatedAllocator : public Allocator
	{
	private:
		Allocator* _primary;
		Allocator* _fallback;
		siz _size_threshold;

		virtual Block Reallocate(void* old_ptr, siz new_size) override
		{
			return {};
		}

	public:
		SegregatedAllocator(Allocator* primary, Allocator* fallback, siz size_threshold):
			_primary(primary),
			_fallback(fallback),
			_size_threshold(size_threshold)
		{
			NP_ENGINE_ASSERT(_primary != nullptr, "we must get a valid primary allocator");
			NP_ENGINE_ASSERT(_fallback != nullptr, "we must get a valid fallback allocator");
			NP_ENGINE_ASSERT(_size_threshold > 0, "we must have a nonzero size threshold");
		}

		virtual siz GetSizeThreshold() const
		{
			return _size_threshold;
		}

		virtual Allocator& GetPrimary()
		{
			return *_primary;
		}

		virtual const Allocator& GetPrimary() const
		{
			return *_primary;
		}

		virtual Allocator& GetFallback()
		{
			return *_fallback;
		}

		virtual const Allocator& GetFallback() const
		{
			return *_fallback;
		}

		virtual bl Contains(const Block& block) override
		{
			return _primary->Contains(block) || _fallback->Contains(block);
		}

		virtual bl Contains(const void* ptr) override
		{
			return _primary->Contains(ptr) || _fallback->Contains(ptr);
		}

		virtual Block Allocate(siz size) override
		{
			Block block;
			if (size <= _size_threshold)
				block = _primary->Allocate(size);
			if (!block.IsValid())
				block = _fallback->Allocate(size);

			return block;
		}

		virtual Block Reallocate(Block& old_block, siz new_size) override
		{
			Block new_block = Allocate(new_size);
			if (Contains(old_block))
			{
				CopyBytes(new_block.Begin(), old_block.Begin(), old_block.size);
				Deallocate(old_block);
				old_block.Invalidate();
			}

			return new_block;
		}

		virtual bl Deallocate(Block& block) override
		{
			return _primary->Contains(block) ? _primary->Contains(block) : _fallback->Deallocate(block);
		}

		virtual bl Deallocate(void* ptr) override
		{
			return _primary->Contains(ptr) ? _primary->Contains(ptr) : _fallback->Deallocate(ptr);
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_SEGEREGATED_ALLOCATOR_HPP */
