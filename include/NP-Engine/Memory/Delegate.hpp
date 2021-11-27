//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/27/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_DELEGATE_HPP
#define NP_ENGINE_DELEGATE_HPP

#include "PadObject.hpp"

namespace np::memory
{
	class Delegate : public PadObject
	{
	protected:
		using InstancePtr = void*;
		using FunctionPtr = void (*)(InstancePtr, Delegate&);

		InstancePtr _instance_ptr;
		FunctionPtr _function_ptr;

		/*
			used to wrap a static function to our function pointer
		*/
		template <void (*Function)(Delegate&)>
		inline static void wrap(InstancePtr, Delegate& delegate)
		{
			return (Function)(delegate);
		}

		/*
			used to wrap a class method to our function pointer
		*/
		template <class C, void (C::*Function)(Delegate&)>
		inline static void wrap(InstancePtr instance, Delegate& delegate)
		{
			return (static_cast<C*>(instance)->*Function)(delegate);
		}

		/*
			copies from the given other Delegate
		*/
		inline void CopyFrom(const Delegate& other)
		{
			PadObject::CopyFrom(other);
			_instance_ptr = other._instance_ptr;
			_function_ptr = other._function_ptr;
		}

	public:
		Delegate(): _instance_ptr(nullptr), _function_ptr(nullptr) {}

		Delegate(const Delegate& other)
		{
			CopyFrom(other);
		}

		/*
			move constructor acts like copy
		*/
		Delegate(Delegate&& other)
		{
			CopyFrom(other);
		}

		virtual ~Delegate() {}

		/*
			connects a static function to our function pointer
		*/
		template <void (*Function)(Delegate&)>
		inline void Connect()
		{
			_instance_ptr = nullptr;
			_function_ptr = &wrap<Function>;
		}

		/*
			connects a class method to our function pointer
		*/
		template <class C, void (C::*Method)(Delegate&)>
		inline void Connect(InstancePtr instance)
		{
			_instance_ptr = instance;
			_function_ptr = &wrap<C, Method>;
		}

		/*
			invokes our function if valid
		*/
		inline void InvokeConnectedFunction()
		{
			if (IsConnected())
			{
				_function_ptr(_instance_ptr, *this);
			}
		}

		/*
			checks if our function pointer is nullptr or not
		*/
		bl IsConnected() const
		{
			return _function_ptr != nullptr;
		}

		/*
			disconnects our instance and function pointers by setting them to nullptr
		*/
		inline void DisconnectFunction()
		{
			_instance_ptr = nullptr;
			_function_ptr = nullptr;
		}

		/*
			call operator calls our invoke method
		*
		inline void operator()()
		{
			Invoke();
		}
		*/

		inline Delegate& operator=(const Delegate& other)
		{
			CopyFrom(other);
			return *this;
		}

		/*
			move assignment - act like copy assignment
		*/
		Delegate& operator=(Delegate&& other)
		{
			CopyFrom(other);
			return *this;
		}
	};
} // namespace np::memory

#endif /* NP_ENGINE_DELEGATE_HPP */
