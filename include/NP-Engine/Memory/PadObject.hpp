//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_PAD_OBJECT_HPP
#define NP_ENGINE_PAD_OBJECT_HPP

#include <utility>
#include <type_traits>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "Allocator.hpp"

namespace np::mem
{
	using CacheLinePadding = SizedBlock<64>;

	class PadObject //TODO: I'm still not a fan of how this object constructs, destructs, and gets data... I feel like there's a better solution
	{
	private:
		CacheLinePadding _padding;

	public:
		template <typename T, typename... Args>
		bl ConstructData(Args&&... args)
		{
			NP_ENGINE_ASSERT(sizeof(T) <= CacheLinePadding::SIZE, "given T must be <= CacheLinePadding::SIZE");
			return mem::Construct<T>(_padding, ::std::forward<Args>(args)...);
		}

		template <typename T>
		bl DestructData()
		{
			bl destructed = mem::Destruct<T>((T*)_padding.allocation);
			((Block)_padding).Zeroize();
			return destructed;
		}

		template <typename T>
		T& GetData()
		{
			return *((T*)_padding.allocation);
		}

		template <typename T>
		const T& GetData() const
		{
			return *((T*)_padding.allocation);
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_PAD_OBJECT_HPP */
