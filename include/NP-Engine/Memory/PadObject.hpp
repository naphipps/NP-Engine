//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_PAD_OBJECT_HPP
#define NP_ENGINE_PAD_OBJECT_HPP

#include <string>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"

namespace np::memory
{
	constexpr static siz CACHE_LINE_SIZE = 64;
	using CacheLinePadding = ui8[CACHE_LINE_SIZE];

	// TODO: for alignment sake, store assigned data at &_padding[0], then check _padding[CACHE_LINE_SIZE-1] for dirty

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
			NP_ASSERT(sizeof(T) <= CACHE_LINE_SIZE - 1,
					  "cannot assign padding struct of size (" + ::std::to_string(sizeof(T)) + ")\nplease keep it <= (" +
						  ::std::to_string(CACHE_LINE_SIZE - 1) + ")");

			*(T*)(&_padding[1]) = object;
			_padding[0] = 1;
		}

		template <typename T>
		const T& RetrieveData() const
		{
			return *(T*)(&_padding[1]);
		}

		template <typename T>
		T& RetrieveData()
		{
			return *(T*)(&_padding[1]);
		}

		bl IsDirty() const
		{
			return _padding[0] != 0;
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
