//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_FALLBACK_ALLOCATOR_HHPP
#define NP_ENGINE_FALLBACK_ALLOCATOR_HHPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "Allocator.hpp"
#include "MemoryFunctions.hpp"

namespace np::mem
{
	class FallbackAllocator : public Allocator
	{
	private:
		Allocator* _primary;
		Allocator* _fallback;

		virtual Block Reallocate(void* old_ptr, siz new_size) override
		{
			return {};
		}

	public:
		FallbackAllocator(Allocator* primary, Allocator* fallback): _primary(primary), _fallback(fallback)
		{
			NP_ENGINE_ASSERT(_primary != nullptr, "we must get a valid primary allocator");
			NP_ENGINE_ASSERT(_fallback != nullptr, "we must get a valid fallback allocator");
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

		virtual bl Contains(const Block& block) const override
		{
			return _primary->Contains(block) || _fallback->Contains(block);
		}

		virtual bl Contains(const void* ptr) const override
		{
			return _primary->Contains(ptr) || _fallback->Contains(ptr);
		}

		virtual Block Allocate(siz size) override
		{
			Block block = _primary->Allocate(size);
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

		bl Deallocate(void* ptr) override
		{
			return _primary->Contains(ptr) ? _primary->Contains(ptr) : _fallback->Deallocate(ptr);
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_FALLBACK_ALLOCATOR_HHPP */
