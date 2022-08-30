//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/27/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_DELEGATE_HPP
#define NP_ENGINE_DELEGATE_HPP

#include <utility>
#include <type_traits>

#include "NP-Engine/Primitive/Primitive.hpp"

#include "PadObject.hpp"

namespace np::mem
{
	template <typename R>
	class DelegateTemplate : public PadObject
	{
	protected:
		using InstancePtr = void*;
		using FunctionPtr = R (*)(InstancePtr, DelegateTemplate<R>&);

		InstancePtr _instance_ptr;
		FunctionPtr _function_ptr;

		/*
			used to wrap a static function to our function pointer
		*/
		template <R (*Function)(DelegateTemplate<R>&)>
		inline static R wrap(InstancePtr, DelegateTemplate<R>& del)
		{
			return (Function)(del);
		}

		/*
			used to wrap a class method to our function pointer
		*/
		template <class C, R (C::*Function)(DelegateTemplate<R>&)>
		inline static R wrap(InstancePtr ptr, DelegateTemplate<R>& del)
		{
			return (static_cast<C*>(ptr)->*Function)(del);
		}

	public:
		DelegateTemplate(): _instance_ptr(nullptr), _function_ptr(nullptr) {}

		DelegateTemplate(const DelegateTemplate<R>& other):
			PadObject(static_cast<const PadObject&>(other)),
			_instance_ptr(other._instance_ptr),
			_function_ptr(other._function_ptr)
		{}

		DelegateTemplate(DelegateTemplate<R>&& other) noexcept:
			PadObject(static_cast<PadObject&&>(other)),
			_instance_ptr(::std::move(other._instance_ptr)),
			_function_ptr(::std::move(other._function_ptr))
		{}

		virtual ~DelegateTemplate() {}

		DelegateTemplate<R>& operator=(const DelegateTemplate<R>& other)
		{
			PadObject::operator=(static_cast<const PadObject&>(other));
			_instance_ptr = other._instance_ptr;
			_function_ptr = other._function_ptr;
			return *this;
		}

		DelegateTemplate<R>& operator=(DelegateTemplate<R>&& other) noexcept
		{
			PadObject::operator=(static_cast<PadObject&&>(other));
			_instance_ptr = ::std::move(other._instance_ptr);
			_function_ptr = ::std::move(other._function_ptr);
			return *this;
		}

		/*
			connects a static function to our function pointer
		*/
		template <R (*Function)(DelegateTemplate<R>&)>
		inline void Connect()
		{
			_instance_ptr = nullptr;
			_function_ptr = &wrap<Function>;
		}

		/*
			connects a class method to our function pointer
		*/
		template <class C, R (C::*Method)(DelegateTemplate<R>&)>
		inline void Connect(InstancePtr ptr)
		{
			_instance_ptr = ptr;
			_function_ptr = &wrap<C, Method>;
		}

		inline R InvokeConnectedFunction()
		{
			if constexpr (::std::is_same_v<void, R>)
			{
				if (IsConnected())
					_function_ptr(_instance_ptr, *this);

				return;
			}
			else
			{
				R r;

				if (IsConnected())
					r = _function_ptr(_instance_ptr, *this);

				return r;
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

	using Delegate = DelegateTemplate<void>;
	using VoidDelegate = DelegateTemplate<void>;
	using BlDelegate = DelegateTemplate<bl>;
	using Ui8Delegate = DelegateTemplate<ui8>;
	using Ui16Delegate = DelegateTemplate<ui16>;
	using Ui32Delegate = DelegateTemplate<ui32>;
	using Ui64Delegate = DelegateTemplate<ui64>;
	using I8Delegate = DelegateTemplate<i8>;
	using I16Delegate = DelegateTemplate<i16>;
	using I32Delegate = DelegateTemplate<i32>;
	using I64Delegate = DelegateTemplate<i64>;
	using FltDelegate = DelegateTemplate<flt>;
	using DblDelegate = DelegateTemplate<dbl>;
	using SizDelegate = DelegateTemplate<siz>;
} // namespace np::mem

#endif /* NP_ENGINE_DELEGATE_HPP */
