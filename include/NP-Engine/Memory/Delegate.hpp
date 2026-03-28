//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/27/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_DELEGATE_HPP
#define NP_ENGINE_MEM_DELEGATE_HPP

#include <utility>
#include <type_traits>

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::mem
{
	template <typename R = void>
	class delegate_template
	{
	public:
		using callback = R(*)(delegate_template<R>&);

	protected:
		siz _id;
		callback _callback;
		void* _payload;

	public:
		delegate_template(): _id(SIZ_MAX), _callback(nullptr), _payload(nullptr) {}

		delegate_template(const delegate_template<R>& other): _id(other._id), _callback(other._callback), _payload(other._payload)
		{}

		delegate_template(delegate_template<R>&& other) noexcept:
			_id(::std::move(other._id)),
			_callback(::std::move(other._callback)),
			_payload(::std::move(other._payload))
		{}

		virtual ~delegate_template() {}

		virtual delegate_template<R>& operator=(const delegate_template<R>& other)
		{
			_id = other._id;
			_callback = other._callback;
			_payload = other._payload;
			return *this;
		}

		virtual delegate_template<R>& operator=(delegate_template<R>&& other) noexcept
		{
			_id = ::std::move(other._id);
			_callback = ::std::move(other._callback);
			_payload = ::std::move(other._payload);
			return *this;
		}

		virtual void set_id(siz id)
		{
			_id = id;
		}

		virtual void SetId(siz id)
		{
			set_id(id);
		}

		virtual siz get_id() const
		{
			return _id;
		}

		virtual siz GetId() const
		{
			return get_id();
		}

		virtual void set_callback(callback c)
		{
			_callback = c;
		}

		virtual void SetCallback(callback c)
		{
			set_callback(c);
		}

		virtual callback get_callback() const
		{
			return _callback;
		}

		virtual callback GetCallback() const
		{
			return _callback;
		}

		virtual void set_payload(void* payload)
		{
			_payload = payload;
		}

		virtual void SetPayload(void* payload)
		{
			set_payload(payload);
		}

		virtual void* get_payload() const
		{
			return _payload;
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
			}
			else
			{
				return _callback ? _callback(*this) : R{};
			}
		}
	};

	using delegate = delegate_template<>;
	using delegate_void = delegate_template<void>;
	using delegate_bl = delegate_template<bl>;
	using delegate_ui8 = delegate_template<ui8>;
	using delegate_ui16 = delegate_template<ui16>;
	using delegate_ui32 = delegate_template<ui32>;
	using delegate_ui64 = delegate_template<ui64>;
	using delegate_i8 = delegate_template<i8>;
	using delegate_i16 = delegate_template<i16>;
	using delegate_i32 = delegate_template<i32>;
	using delegate_i64 = delegate_template<i64>;
	using delegate_flt = delegate_template<flt>;
	using delegate_dbl = delegate_template<dbl>;
	using delegate_siz = delegate_template<siz>;
} // namespace np::mem

#endif /* NP_ENGINE_MEM_DELEGATE_HPP */
