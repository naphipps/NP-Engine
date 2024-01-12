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

namespace np::mem
{
	template <typename R>
	class DelegateTemplate
	{
	public:
		using Callback = R(*)(DelegateTemplate<R>&);

	protected:
		siz _id;
		Callback _callback;
		void* _payload;

	public:
		DelegateTemplate() : _id(-1), _callback(nullptr), _payload(nullptr) {}

		DelegateTemplate(const DelegateTemplate<R>& other):
			_id(other._id),
			_callback(other._callback),
			_payload(other._payload)
		{}

		DelegateTemplate(DelegateTemplate<R>&& other) noexcept :
			_id(::std::move(other._id)),
			_callback(::std::move(other._callback)),
			_payload(::std::move(other._payload))
		{}

		virtual ~DelegateTemplate() {}

		DelegateTemplate<R>& operator=(const DelegateTemplate<R>& other)
		{
			_id = other._id;
			_callback = other._callback;
			_payload = other._payload;
			return *this;
		}

		DelegateTemplate<R>& operator=(DelegateTemplate<R>&& other) noexcept
		{
			_id = ::std::move(other._id);
			_callback = ::std::move(other._callback);
			_payload = ::std::move(other._payload);
			return *this;
		}

		virtual void SetId(siz id)
		{
			_id = id;
		}

		virtual siz GetId() const
		{
			return _id;
		}

		virtual void SetCallback(Callback callback)
		{
			_callback = callback;
		}

		virtual Callback GetCallback() const
		{
			return _callback;
		}

		virtual void SetPayload(void* payload)
		{
			_payload = payload;
		}

		virtual void* GetPayload() const
		{
			return _payload;
		}

		virtual R operator()()
		{
			if constexpr (::std::is_same_v<void, R>)
			{
				if (_callback)
					_callback(*this);

				return;
			}
			else
			{
				R r;

				if (_callback)
					r = _callback(*this);

				return r;
			}
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
