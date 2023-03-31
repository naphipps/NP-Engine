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
	constexpr static siz CACHE_LINE_SIZE = 64;
	using CacheLinePadding = ui8[CACHE_LINE_SIZE];

	class PadObject
	{
	private:
		CacheLinePadding _padding;

	public:
		template <typename T, typename... Args>
		bl ConstructData(Args&&... args)
		{
			NP_ENGINE_ASSERT(sizeof(T) <= CACHE_LINE_SIZE, "given T must be <= CACHE_LINE_SIZE");
			Block block{ (void*)_padding, CACHE_LINE_SIZE };
			return mem::Construct<T>(block, ::std::forward<Args>(args)...);
		}

		template <typename T>
		bl DestructData()
		{
			return Destruct<T>((T*)_padding);
		}

		template <typename T>
		T& GetData()
		{
			return *((T*)_padding);
		}

		template <typename T>
		const T& GetData() const
		{
			return *((T*)_padding);
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_PAD_OBJECT_HPP */
