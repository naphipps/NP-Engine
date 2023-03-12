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
	public:
		using Callback = R (*)(void*, DelegateTemplate<R>&);

	protected:
		void* _caller;
		Callback _callback;

	public:
		DelegateTemplate(): _caller(nullptr), _callback(nullptr) {}

		DelegateTemplate(const DelegateTemplate<R>& other):
			PadObject(static_cast<const PadObject&>(other)),
			_caller(other._caller),
			_callback(other._callback)
		{}

		DelegateTemplate(DelegateTemplate<R>&& other) noexcept:
			PadObject(static_cast<PadObject&&>(other)),
			_caller(::std::move(other._caller)),
			_callback(::std::move(other._callback))
		{}

		virtual ~DelegateTemplate() {}

		DelegateTemplate<R>& operator=(const DelegateTemplate<R>& other)
		{
			PadObject::operator=(static_cast<const PadObject&>(other));
			_caller = other._caller;
			_callback = other._callback;
			return *this;
		}

		DelegateTemplate<R>& operator=(DelegateTemplate<R>&& other) noexcept
		{
			PadObject::operator=(static_cast<PadObject&&>(other));
			_caller = ::std::move(other._caller);
			_callback = ::std::move(other._callback);
			return *this;
		}

		virtual void SetCallback(void* caller, Callback callback)
		{
			_caller = caller;
			_callback = callback;
		}

		virtual void SetCallback(Callback callback)
		{
			SetCallback(nullptr, callback);
		}

		virtual R operator()()
		{
			if constexpr (::std::is_same_v<void, R>)
			{
				if (_callback)
					_callback(_caller, *this);

				return;
			}
			else
			{
				R r;

				if (_callback)
					r = _callback(_caller, *this);

				return r;
			}

			//return _callback ? _callback(_caller, *this) : R; //TODO: does this work with void??
		}

		virtual void Clear() override
		{
			SetCallback(nullptr, nullptr);
			PadObject::Clear();
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
