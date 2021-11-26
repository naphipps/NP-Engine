//
//  PadObject.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/19/21.
//

#ifndef NP_ENGINE_PAD_OBJECT_HPP
#define NP_ENGINE_PAD_OBJECT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"

#include "ConcurrencyConstants.hpp"

namespace np
{
	namespace concurrency
	{
		class PadObject
		{
		protected:
			CacheLinePadding _padding;

			/**
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
			/**
			 constructor
			 */
			PadObject()
			{
				Clear();
			}

			/**
			 copy constructor
			 */
			PadObject(const PadObject& other)
			{
				CopyFrom(other);
			}

			/**
			 move constructor - behaves like copy
			 */
			PadObject(PadObject&& other)
			{
				CopyFrom(other);
			}

			/**
			 deconstructor
			 */
			~PadObject() = default;

			/**
			 assigns the given value or struct into the padding of this JobFunction
			 */
			template <typename T>
			void AssignData(const T& object)
			{
				NP_ASSERT(sizeof(T) <= CACHE_LINE_SIZE - 1,
						  "cannot assign padding struct of size (" + to_str(sizeof(T)) + ")\nplease keep it <= (" +
							  to_str(CACHE_LINE_SIZE - 1) + ")");

				*(T*)(&_padding[1]) = object;
				_padding[0] = 1;
			}

			/**
			 retrieves the value or struct from the padding of this JobFunction
			 */
			template <typename T>
			const T& RetrieveData() const
			{
				return *(T*)(&_padding[1]);
			}

			/**
			 retrieves the value or struct from the padding of this JobFunction
			 */
			template <typename T>
			T& RetrieveData()
			{
				return *(T*)(&_padding[1]);
			}

			/**
			 indicates if our padding is dirty aka we have data stored there
			 */
			bl IsDirty() const
			{
				return _padding[0] != 0;
			}

			/**
			 zeroizes our padding
			 */
			void Clear()
			{
				for (siz i = 0; i < CACHE_LINE_SIZE; i++)
				{
					_padding[i] = 0;
				}
			}

			/**
			 copy assignment
			 */
			PadObject& operator=(const PadObject& other)
			{
				CopyFrom(other);
				return *this;
			}

			/**
			 move assignment - behaves like copy
			 */
			PadObject& operator=(PadObject&& other)
			{
				CopyFrom(other);
				return *this;
			}
		};
	} // namespace concurrency
} // namespace np

#endif /* NP_ENGINE_PAD_OBJECT_HPP */
