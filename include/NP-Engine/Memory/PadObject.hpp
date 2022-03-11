//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_PAD_OBJECT_HPP
#define NP_ENGINE_PAD_OBJECT_HPP

#include <string>
#include <type_traits>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::memory
{
	constexpr static siz CACHE_LINE_SIZE = 64;
	using CacheLinePadding = ui8[CACHE_LINE_SIZE];

	class PadObject
	{
	protected:
		CacheLinePadding _padding;

		void CopyFrom(const PadObject& other)
		{
			for (siz i = 0; i < CACHE_LINE_SIZE; i++)
			{
				_padding[i] = other._padding[i];
			}
		}

	public:
		PadObject()
		{
			Clear();
		}

		PadObject(const PadObject& other)
		{
			CopyFrom(other);
		}

		PadObject(PadObject&& other)
		{
			CopyFrom(other);
		}

		template <typename T>
		void AssignData(const T& object)
		{
			NP_ENGINE_ASSERT(sizeof(T) <= CACHE_LINE_SIZE - 1,
							 "cannot assign padding struct of size (" + ::std::to_string(sizeof(T)) + ")\nplease keep it <= (" +
								 ::std::to_string(CACHE_LINE_SIZE - 1) + ")");

			NP_ENGINE_ASSERT(::std::is_copy_assignable_v<T>, "T must be copy assignable");

			*((T*)_padding) = object;
			_padding[CACHE_LINE_SIZE - 1] = 1;
		}

		template <typename T>
		const T& RetrieveData() const
		{
			return *((T*)_padding);
		}

		template <typename T>
		T& RetrieveData()
		{
			return *((T*)_padding);
		}

		bl IsDirty() const
		{
			return _padding[CACHE_LINE_SIZE - 1] != 0;
		}

		void Clear()
		{
			for (siz i = 0; i < CACHE_LINE_SIZE; i++)
			{
				_padding[i] = 0;
			}
		}

		PadObject& operator=(const PadObject& other)
		{
			CopyFrom(other);
			return *this;
		}

		PadObject& operator=(PadObject&& other)
		{
			CopyFrom(other);
			return *this;
		}
	};
} // namespace np::memory

#endif /* NP_ENGINE_PAD_OBJECT_HPP */
