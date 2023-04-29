//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/28/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MUTEXED_WRAPPER_HPP
#define NP_ENGINE_MUTEXED_WRAPPER_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"

#include "SmartPtr.hpp"

namespace np::mem
{
	template <typename T>
	class MutexedWrapper
	{
	protected:
		Mutex _m;
		T _object;
		
	public:
		class Access
		{
		protected:
			Lock _l;
			T& _reference;

		public:
			Access(Mutex& m, T& reference) : _l(m), _reference(reference) {}

			T& operator*() const
			{
				return _reference;
			}

			T* operator->() const
			{
				return &_reference;
			}
		};
		
		template <typename... Args>
		MutexedWrapper(Args&&... args) : _object(::std::forward<Args>(args)...) {}

		Access GetAccess()
		{
			return { _m, _object };
		}
	};
}

#endif /* NP_ENGINE_MUTEXED_WRAPPER_HPP */