//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_PAD_OBJECT_HPP
#define NP_ENGINE_PAD_OBJECT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include <string>

namespace np::memory
{
	constexpr static siz CACHE_LINE_SIZE = 64;
	using CacheLinePadding = ui8[CACHE_LINE_SIZE];

	class PadObject
	{
	protected:
		CacheLinePadding _padding;

		/*
			copies padding from other
		*/
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

		/*
			move behaves like copy
		*/
		PadObject(PadObject&& other)
		{
			CopyFrom(other);
		}

		virtual ~PadObject(){};

		/*
			assigns the given value or struct into the padding
		*/
		template <typename T>
		void AssignData(const T& object)
		{
			NP_ASSERT(sizeof(T) <= CACHE_LINE_SIZE - 1,
					  "cannot assign padding struct of size (" + ::std::to_string(sizeof(T)) + ")\nplease keep it <= (" +
						  ::std::to_string(CACHE_LINE_SIZE - 1) + ")");

			*(T*)(&_padding[1]) = object;
			_padding[0] = 1;
		}

		/*
			retrieves the value or struct from the padding
		*/
		template <typename T>
		const T& RetrieveData() const
		{
			return *(T*)(&_padding[1]);
		}

		/*
			retrieves the value or struct from the padding
		*/
		template <typename T>
		T& RetrieveData()
		{
			return *(T*)(&_padding[1]);
		}

		/*
			indicates if our padding is dirty aka we have data stored there
		*/
		bl IsDirty() const
		{
			return _padding[0] != 0;
		}

		/*
			zeroizes our padding
		*/
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

		/*
			move assignment - behaves like copy
		*/
		PadObject& operator=(PadObject&& other)
		{
			CopyFrom(other);
			return *this;
		}
	};
} // namespace np::memory

#endif /* NP_ENGINE_PAD_OBJECT_HPP */
