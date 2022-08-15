//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/27/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_DELEGATE_HPP
#define NP_ENGINE_DELEGATE_HPP

#include <utility>

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
		inline static void wrap(InstancePtr, Delegate& delegate) // TODO: refactor delegate since it is a keyword
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

	public:
		Delegate(): _instance_ptr(nullptr), _function_ptr(nullptr) {}

		Delegate(const Delegate& other):
			PadObject(static_cast<const PadObject&>(other)),
			_instance_ptr(other._instance_ptr),
			_function_ptr(other._function_ptr)
		{}

		Delegate(Delegate&& other) noexcept:
			PadObject(static_cast<PadObject&&>(other)),
			_instance_ptr(::std::move(other._instance_ptr)),
			_function_ptr(::std::move(other._function_ptr))
		{}

		virtual ~Delegate() {}

		Delegate& operator=(const Delegate& other)
		{
			PadObject::operator=(static_cast<const PadObject&>(other));
			_instance_ptr = other._instance_ptr;
			_function_ptr = other._function_ptr;
			return *this;
		}

		Delegate& operator=(Delegate&& other) noexcept
		{
			PadObject::operator=(static_cast<PadObject&&>(other));
			_instance_ptr = ::std::move(other._instance_ptr);
			_function_ptr = ::std::move(other._function_ptr);
			return *this;
		}

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

		inline void InvokeConnectedFunction()
		{
			if (IsConnected())
			{
				_function_ptr(_instance_ptr, *this);
			}
		}

		bl IsConnected() const
		{
			return _function_ptr != nullptr;
		}

		inline void DisconnectFunction()
		{
			_instance_ptr = nullptr;
			_function_ptr = nullptr;
		}
	};
} // namespace np::memory

#endif /* NP_ENGINE_DELEGATE_HPP */
