//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/28/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_LOCKING_WRAPPER_HPP
#define NP_ENGINE_LOCKING_WRAPPER_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "SmartPtr.hpp"

namespace np::mem
{
	template <typename T>
	class LockingWrapper
	{
	public:
		class Access
		{
		protected:
			mem::sptr<Lock> _lock;
			T& _reference;

		public:
			Access(mem::sptr<Lock> lock, T& reference) : _lock(lock), _reference(reference) {}

			Access(const Access& other) : _lock(other._lock), _reference(other._reference) {}

			T& operator*() const
			{
				return _reference;
			}

			T* operator->() const
			{
				return &_reference;
			}
		};
		
	protected:
		Mutex _m;
		T _object;

	public:
		Access GetAccess(Allocator& allocator)
		{
			return Access(mem::create_sptr<Lock>(allocator, _m), _object);
		}
	};
}

#endif /* NP_ENGINE_LOCKING_WRAPPER_HPP */