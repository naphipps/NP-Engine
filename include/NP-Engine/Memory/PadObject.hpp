//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_PAD_OBJECT_HPP
#define NP_ENGINE_PAD_OBJECT_HPP

#include <utility>
#include <string>
#include <type_traits>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::mem
{
	constexpr static siz CACHE_LINE_SIZE = 64;
	using CacheLinePadding = ui8[CACHE_LINE_SIZE];

	class PadObject
	{
	protected:
		CacheLinePadding _padding;

	public:
		PadObject()
		{
			Clear();
		}

		PadObject(const PadObject& other)
		{
			for (siz i = 0; i < CACHE_LINE_SIZE; i++)
				_padding[i] = other._padding[i];
		}

		PadObject(PadObject&& other) noexcept
		{
			for (siz i = 0; i < CACHE_LINE_SIZE; i++)
				_padding[i] = ::std::move(other._padding[i]);
		}

		virtual ~PadObject() {}

		PadObject& operator=(const PadObject& other)
		{
			for (siz i = 0; i < CACHE_LINE_SIZE; i++)
				_padding[i] = other._padding[i];
			return *this;
		}

		PadObject& operator=(PadObject&& other) noexcept
		{
			for (siz i = 0; i < CACHE_LINE_SIZE; i++)
				_padding[i] = ::std::move(other._padding[i]);
			return *this;
		}

		//TODO: refactor AssignData and RetrieveData to SetData and GetData

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
				_padding[i] = 0;
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_PAD_OBJECT_HPP */
