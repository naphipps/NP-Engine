//
//  FallbackAllocator.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/19/21.
//

#ifndef NP_ENGINE_FALLBACK_ALLOCATOR_HHPP
#define NP_ENGINE_FALLBACK_ALLOCATOR_HHPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "Allocator.hpp"

namespace np
{
	namespace memory
	{
		/**
		 fallback allocator - fallback defaults to CAllocator
		 */
		class FallbackAllocator : public Allocator
		{
		private:
			Allocator* _primary;
			Allocator* _fallback;

		public:
			/**
			 constructor
			 */
			FallbackAllocator(Allocator* primary, Allocator* fallback): _primary(primary), _fallback(fallback)
			{
				NP_ASSERT(_primary != nullptr, "we must get a valid primary allocator");
				NP_ASSERT(_fallback != nullptr, "we must get a valid fallback allocator");
			}

			/**
			 deconstructor
			 */
			~FallbackAllocator() = default;

			/**
			 gets the primary allocator
			 */
			virtual Allocator& GetPrimary()
			{
				return *_primary;
			}

			/**
			 gets the primary allocator
			 */
			virtual const Allocator& GetPrimary() const
			{
				return *_primary;
			}

			/**
			 gets the fallback allocator
			 */
			virtual Allocator& GetFallback()
			{
				return *_fallback;
			}

			/**
			 gets the fallback allocator
			 */
			virtual const Allocator& GetFallback() const
			{
				return *_fallback;
			}

			/**
			 checks if we contain given block
			 */
			virtual bl Contains(const Block& block) const override
			{
				return _primary->Contains(block) || _fallback->Contains(block);
			}

			/**
			 checks if we contain given ptr
			 */
			virtual bl Contains(const void* ptr) const override
			{
				return _primary->Contains(ptr) || _fallback->Contains(ptr);
			}

			/**
			 allocates a Block of given size
			 */
			virtual Block Allocate(siz size) override
			{
				Block block = _primary->Allocate(size);
				if (!block.IsValid())
				{
					block = _fallback->Allocate(size);
				}
				return block;
			}

			/**
			 deallocates a given Block
			 */
			virtual bl Deallocate(Block& block) override
			{
				return _primary->Contains(block) ? _primary->Contains(block) : _fallback->Deallocate(block);
			}

			/**
			 deallocates a block given the ptr
			 */
			bl Deallocate(void* ptr) override
			{
				return _primary->Contains(ptr) ? _primary->Contains(ptr) : _fallback->Deallocate(ptr);
			}
		};
	}; // namespace memory
} // namespace np

#endif /* NP_ENGINE_FALLBACK_ALLOCATOR_HHPP */
