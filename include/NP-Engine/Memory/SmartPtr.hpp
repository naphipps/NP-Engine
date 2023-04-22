//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/29/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_SMART_PTR_HPP
#define NP_ENGINE_SMART_PTR_HPP

#include <type_traits>
#include <utility>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "Allocator.hpp"

/*
	<https://www.quora.com/Are-the-classic-pointers-still-used-in-modern-C-or-are-they-being-replaced-more-and-more-by-the-smart-pointers>
	<https://www.quora.com/When-should-RAII-smart-pointers-and-raw-pointers-be-used-in-C-When-should-we-not-use-RAII-in-modern-C/answer/Drew-Eckhardt>
*/

namespace np::mem
{
	struct smart_ptr_resource_base
	{
		atm_siz strong_counter = 0;
		atm_siz weak_counter = 0;

		virtual ~smart_ptr_resource_base() = default;

		virtual void destroy_object() = 0;
		virtual void destroy_self() = 0;
		virtual void* get_object_ptr() const = 0;
	};

	template <typename T>
	struct smart_ptr_destroyer
	{
		virtual void destruct_object(T* ptr)
		{
			mem::Destruct<T>(ptr);
		}

		virtual void destruct_resource(smart_ptr_resource_base* ptr)
		{
			mem::Destruct<smart_ptr_resource_base>(ptr);
		}

		virtual void deallocate_object(T* ptr) = 0;

		virtual void deallocate_resource(smart_ptr_resource_base* ptr) = 0;
	};

	template <typename T>
	class smart_ptr_allocator_destroyer : public smart_ptr_destroyer<T>
	{
	protected:
		Allocator& _allocator;

	public:
		smart_ptr_allocator_destroyer(Allocator& allocator) : _allocator(allocator) {}

		smart_ptr_allocator_destroyer(const smart_ptr_allocator_destroyer& other) : _allocator(other._allocator) {}

		smart_ptr_allocator_destroyer(smart_ptr_allocator_destroyer&& other) noexcept : _allocator(other._allocator) {}

		virtual void deallocate_object(T* ptr) override
		{
			_allocator.Deallocate(ptr);
		}

		virtual void deallocate_resource(smart_ptr_resource_base* ptr) override
		{
			_allocator.Deallocate(ptr);
		}
	};

	template <typename T>
	class smart_ptr_contiguous_destroyer : public smart_ptr_allocator_destroyer<T>
	{
	private:
		using base = smart_ptr_allocator_destroyer<T>;

	public:
		smart_ptr_contiguous_destroyer(Allocator& allocator) : base(allocator) {}

		smart_ptr_contiguous_destroyer(const smart_ptr_contiguous_destroyer& other) : base(other._allocator) {}

		smart_ptr_contiguous_destroyer(smart_ptr_contiguous_destroyer&& other) noexcept : base(other._allocator) {}

		virtual void deallocate_object(T* ptr) override {}
	};

	template <typename T, typename D = smart_ptr_allocator_destroyer<T>>
	class smart_ptr_resource : public smart_ptr_resource_base
	{
	private:
		NP_ENGINE_STATIC_ASSERT((::std::is_copy_constructible_v<D> || ::std::is_move_constructible_v<D>), "(D)estroyer must be copy constructible or move constructible");
		NP_ENGINE_STATIC_ASSERT((::std::is_base_of_v<smart_ptr_destroyer<T>, D>), "(D)estroyer must derive from smart_ptr_destroyer<T>");

	public:
		using destroyer_type = D;

	protected:
		destroyer_type _destroyer;
		T* _object;

	public:
		smart_ptr_resource(destroyer_type destroyer_type, T* object) : _destroyer(destroyer_type), _object(object) {}

		virtual void destroy_object() override
		{
			_destroyer.destruct_object(_object);
			_destroyer.deallocate_object(_object);
			_object = nullptr;
		}

		virtual void destroy_self() override
		{
			_destroyer.destruct_resource(this);
			_destroyer.deallocate_resource(this);
		}

		void* get_object_ptr() const override
		{
			return _object;
		}
	};

	template <typename T, typename R>
	struct smart_ptr_contiguous_block
	{
		NP_ENGINE_STATIC_ASSERT((::std::is_base_of_v<smart_ptr_resource_base, R>), "(R)esource must derive from smart_ptr_resource_base");

		using resource_block_type = SizedBlock<sizeof(R)>;
		using object_block_type = SizedBlock<sizeof(T)>;

		resource_block_type resource_block;
		object_block_type object_block;
	};

	template <typename T>
	class smart_ptr
	{
	private:
		template <typename U>
		friend class smart_ptr;

	protected:
		smart_ptr_resource_base* _resource;

		atm_siz* get_strong_counter_ptr() const
		{
			return _resource ? mem::AddressOf(_resource->strong_counter) : nullptr;
		}

		atm_siz* get_weak_counter_ptr() const
		{
			return _resource ? mem::AddressOf(_resource->weak_counter) : nullptr;
		}

		virtual void increment_strong_counter()
		{
			atm_siz* strong_counter_ptr = get_strong_counter_ptr();
			if (strong_counter_ptr)
				strong_counter_ptr->fetch_add(1, mo_release);
		}

		virtual void increment_weak_counter()
		{
			atm_siz* weak_counter_ptr = get_weak_counter_ptr();
			if (weak_counter_ptr)
				weak_counter_ptr->fetch_add(1, mo_release);
		}

		virtual void decrement_strong_counter()
		{
			atm_siz* strong_counter_ptr = get_strong_counter_ptr();
			if (strong_counter_ptr)
			{
				siz prev_strong_count = strong_counter_ptr->fetch_sub(1, mo_release);
				if (prev_strong_count == 1)
					_resource->destroy_object();
			}
		}

		virtual void decrement_weak_counter()
		{
			atm_siz* weak_counter_ptr = get_weak_counter_ptr();
			if (weak_counter_ptr)
			{
				siz prev_weak_counter = weak_counter_ptr->fetch_sub(1, mo_release);
				if (prev_weak_counter == 1)
					_resource->destroy_self();
			}
		}

		smart_ptr() : _resource(nullptr) {}

		smart_ptr(nptr) : smart_ptr() {}

		smart_ptr(smart_ptr_resource_base* resource) : _resource(resource) {}

		smart_ptr(const smart_ptr<T>& other) : _resource(other._resource) {}

		smart_ptr(smart_ptr<T>&& other) noexcept : _resource(::std::move(other._resource))
		{
			other._resource = nullptr;
		}

		template <typename U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, bl> = true>
		smart_ptr(const smart_ptr<U>& other) : smart_ptr(other._resource) {}

		template <typename U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, bl> = true>
		smart_ptr(smart_ptr<U>&& other) noexcept : smart_ptr(::std::move(other._resource))
		{
			other._resource = nullptr;
		}

	public:

		virtual ~smart_ptr() {}

		virtual void reset() = 0;

		siz get_strong_count() const
		{
			atm_siz* strong_counter_ptr = get_strong_counter_ptr();
			return strong_counter_ptr ? strong_counter_ptr->load(mo_acquire) : 0;
		}

		siz get_weak_count() const
		{
			atm_siz* weak_counter_ptr = get_weak_counter_ptr();
			return weak_counter_ptr ? weak_counter_ptr->load(mo_acquire) : 0;
		}
	};

	/*
		Strong Ptr (sptr) ensures the existance of the object
	*/
	template <typename T>
	class sptr : public smart_ptr<T>
	{
	protected:
		template <typename U>
		friend class wptr;

		template <typename U>
		friend class sptr;

		using base = smart_ptr<T>;

		T* get_object_ptr() const
		{
			return base::_resource ? (T*)base::_resource->get_object_ptr() : nullptr;
		}

	public:

		sptr() : base() {}
			
		sptr(nptr): base() {}

		sptr(smart_ptr_resource_base* resource) : base(resource)
		{
			base::increment_strong_counter();
			base::increment_weak_counter();
		}

		sptr(const sptr<T>& other) : base(other)
		{
			base::increment_strong_counter();
			base::increment_weak_counter();
		}

		sptr(sptr<T>&& other) noexcept : base(::std::move(other)) {}

		template <typename U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, bl> = true>
		sptr(const sptr<U>& other): base(other)
		{
			base::increment_strong_counter();
			base::increment_weak_counter();
		}

		template <typename U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, bl> = true>
		sptr(sptr<U>&& other) noexcept : base(::std::move(other)) {}

		virtual ~sptr()
		{
			reset();
		}

		sptr<T>& operator=(const sptr<T>& other)
		{
			reset();
			base::_resource = other._resource;
			base::increment_strong_counter();
			base::increment_weak_counter();
			return *this;
		}

		sptr<T>& operator=(sptr<T>&& other) noexcept
		{
			reset();
			base::_resource = ::std::move(other._resource);
			other._resource = nullptr;
			return *this;
		}

		template <typename U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, bl> = true>
		sptr<T>& operator=(const sptr<U>& other)
		{
			reset();
			base::_resource = other._resource;
			base::increment_strong_counter();
			base::increment_weak_counter();
			return *this;
		}

		template <typename U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, bl> = true>
		sptr<T>& operator=(sptr<U>&& other) noexcept
		{
			reset();
			base::_resource = ::std::move(other._resource);
			other._resource = nullptr;
			return *this;
		}

		sptr<T>& operator=(nptr)
		{
			reset();
			return *this;
		}

		T& operator*() const
		{
			return *get_object_ptr();
		}

		T* operator->() const
		{
			return get_object_ptr();
		}

		bl operator==(const sptr<T>& other) const
		{
			return get_object_ptr() == other.get_object_ptr();
		}

		operator bl() const
		{
			return get_object_ptr();
		}

		void reset() override
		{
			base::decrement_strong_counter();
			base::decrement_weak_counter();
			base::_resource = nullptr;
		}
	};

	/*
		Weak Ptr (wptr) does not ensure existance of the object, but can indicate if it is expired
	*/
	template <typename T>
	class wptr : public smart_ptr<T>
	{
	protected:
		template <typename U>
		friend class wptr;

		using base = smart_ptr<T>;

		virtual void increment_strong_counter() override {}

		virtual void decrement_strong_counter() override {}

	public:

		wptr() :base(nullptr) {}

		wptr(nptr) : base() {}

		wptr(const sptr<T>& other): base(other)
		{
			base::increment_weak_counter();
		}

		wptr(const wptr<T>& other) : base(other)
		{
			base::increment_weak_counter();
		}

		wptr(wptr<T>&& other) noexcept : base(::std::move(other)) {}

		template <typename U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, bl> = true>
		wptr(const sptr<U>& other) : base(other)
		{
			base::increment_weak_counter();
		}

		template <typename U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, bl> = true>
		wptr(const wptr<U>& other) : base(other)
		{
			base::increment_weak_counter();
		}

		template <typename U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, bl> = true>
		wptr(wptr<U>&& other) noexcept : base(::std::move(other)) {}

		virtual ~wptr()
		{
			reset();
		}

		wptr<T>& operator=(const sptr<T>& other)
		{
			reset();
			base::_resource = other._resource;
			base::increment_weak_counter();
			return *this;
		}

		wptr<T>& operator=(const wptr<T>& other)
		{
			reset();
			base::_resource = other._resource;
			base::increment_weak_counter();
			return *this;
		}

		wptr<T>& operator=(wptr<T>&& other) noexcept
		{
			reset();
			base::_resource = ::std::move(other._resource);
			other._resource = nullptr;
			return *this;
		}

		template <typename U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, bl> = true>
		wptr<T>& operator=(const sptr<U>& other)
		{
			reset();
			base::_resource = other._resource;
			base::increment_weak_counter();
			return *this;
		}

		template <typename U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, bl> = true>
		wptr<T>& operator=(const wptr<U>& other)
		{
			reset();
			base::_resource = other._resource;
			base::increment_weak_counter();
			return *this;
		}

		template <typename U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, bl> = true>
		wptr<T>& operator=(wptr<U>&& other) noexcept
		{
			reset();
			base::_resource = ::std::move(other._resource);
			other._resource = nullptr;
			return *this;
		}

		wptr<T>& operator=(nptr)
		{
			reset();
			return *this;
		}

		void reset() override
		{
			base::decrement_weak_counter();
			base::_resource = nullptr;
		}

		bl is_expired() const
		{
			return base::get_strong_count() == 0;
		}

		sptr<T> get_sptr() const
		{
			return mem::sptr<T>(is_expired() ? nullptr : base::_resource);
		}
	};
	
	template <typename T, typename... Args>
	constexpr sptr<T> create_sptr(Allocator& allocator, Args&&... args)
	{
		using destroyer_type = smart_ptr_contiguous_destroyer<T>;
		using resource_type = smart_ptr_resource<T, destroyer_type>;
		using contiguous_block_type = smart_ptr_contiguous_block<T, resource_type>;
		
		resource_type* resource = nullptr;
		contiguous_block_type* contiguous_block = mem::Create<contiguous_block_type>(allocator);
		if (contiguous_block)
		{
			T* object = mem::Construct<T>(contiguous_block->object_block, ::std::forward<Args>(args)...);
			resource = mem::Construct<resource_type>(contiguous_block->resource_block, destroyer_type(allocator), object);
		}
		return sptr<T>(resource);
	}
} // namespace np::mem

namespace std
{
	template <typename T>
	struct hash<::np::mem::sptr<T>>
	{
		siz operator()(::np::mem::sptr<T> ptr) const noexcept
		{
			return ptr ? (siz)(::np::mem::AddressOf(*ptr)) : 0;
		}
	};
} // namespace std

#endif /* NP_ENGINE_SMART_PTR_HPP */