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

#include "TraitAllocator.hpp"
#include "MemoryFunctions.hpp"

namespace np::mem
{
	struct smart_ptr_resource_base
	{
		atm_siz reference_counter = 0;
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

		virtual void deallocate_object(T* ptr)
		{
			TraitAllocator allocator;
			allocator.Deallocate(ptr);
		}

		virtual void destruct_resource(smart_ptr_resource_base* ptr)
		{
			mem::Destruct<smart_ptr_resource_base>(ptr);
		}

		virtual void deallocate_resource(smart_ptr_resource_base* ptr)
		{
			TraitAllocator allocator;
			allocator.Deallocate(ptr);
		}
	};

	template <typename T, typename D = smart_ptr_destroyer<T>>
	class smart_ptr_resource : public smart_ptr_resource_base
	{
	private:
		NP_ENGINE_STATIC_ASSERT(::std::is_copy_constructible_v<D>, "(D)estroyer must be copy constructible");
		NP_ENGINE_STATIC_ASSERT(::std::is_move_constructible_v<D>, "(D)estroyer must be move constructible");
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

	template <typename T>
	class smart_allocator_destroyer : public smart_ptr_destroyer<T>
	{
	protected:
		Allocator& _allocator;

	public:
		smart_allocator_destroyer(Allocator& allocator) : _allocator(allocator) {}

		smart_allocator_destroyer(const smart_allocator_destroyer& other) : _allocator(other._allocator) {}

		smart_allocator_destroyer(smart_allocator_destroyer&& other) noexcept : _allocator(other._allocator) {}

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
	class smart_contiguous_destroyer : public smart_allocator_destroyer<T>
	{
	private:
		using base = smart_allocator_destroyer<T>;

	public:
		smart_contiguous_destroyer(Allocator& allocator) : base(allocator) {}

		smart_contiguous_destroyer(const smart_contiguous_destroyer& other) : base(other._allocator) {}

		smart_contiguous_destroyer(smart_contiguous_destroyer&& other) noexcept : base(other._allocator) {}

		virtual void deallocate_object(T* ptr) override {}

		virtual void deallocate_resource(smart_ptr_resource_base* ptr) override
		{
			_allocator.Deallocate(ptr);
		}
	};

	template <typename T>
	class uptr
	{
	public:
		using destroyer_type = smart_ptr_destroyer<T>;

	private:
		T* _object;
		destroyer_type _destroyer;

	public:

		uptr() : _object(nullptr), _destroyer() {}

		uptr(nptr): uptr() {}

		template <typename... Args>
		uptr(destroyer_type destroyer_type, T* object): _object(object), _destroyer(destroyer_type) {}

		uptr(uptr<T>&& other) noexcept : _object(::std::move(other._object)), _destroyer(::std::move(other._destroyer)) {}
		
		~uptr()
		{
			reset();
		}

		uptr<T>& operator=(uptr<T>&& other) noexcept
		{
			reset();
			_object = ::std::move(other._object);
			other._object = nullptr;
			return *this;
		}

		void reset()
		{
			if (_object)
			{
				_destroyer.destruct_object(_object);
				_destroyer.deallocate_object(_object);
				_object = nullptr;
			}
		}

		T& operator*() const
		{
			return *_object;
		}

		T* operator->() const
		{
			return _object;
		}

		bl operator==(const uptr<T>& other) const
		{
			return _object == other._object;
		}

		operator bl() const
		{
			return _object;
		}
	};

	template <typename T>
	class smart_ptr
	{
	private:
		template <typename U>
		friend class smart_ptr;

	protected:
			
		smart_ptr_resource_base* _resource;

		atm_siz* get_reference_counter_ptr() const
		{
			return _resource ? mem::AddressOf(_resource->reference_counter) : nullptr;
		}

		atm_siz* get_weak_counter_ptr() const
		{
			return _resource ? mem::AddressOf(_resource->weak_counter) : nullptr;
		}

		virtual void increment_reference_counter()
		{
			atm_siz* reference_counter_ptr = get_reference_counter_ptr();
			if (reference_counter_ptr)
				(*reference_counter_ptr)++;
		}

		virtual void increment_weak_counter()
		{
			atm_siz* weak_counter_ptr = get_weak_counter_ptr();
			if (weak_counter_ptr)
				(*weak_counter_ptr)++;
		}

		virtual void decrement_reference_counter()
		{
			atm_siz* reference_counter_ptr = get_reference_counter_ptr();
			if (reference_counter_ptr)
			{
				siz prev_reference_count = reference_counter_ptr->load(mo_acquire);
				while (prev_reference_count > 0 && !reference_counter_ptr->compare_exchange_weak(prev_reference_count, prev_reference_count - 1, mo_release, mo_relaxed))
				{}

				if (prev_reference_count == 1)
					_resource->destroy_object();
			}
		}

		virtual void decrement_weak_counter()
		{
			atm_siz* weak_counter_ptr = get_weak_counter_ptr();
			if (weak_counter_ptr)
			{
				siz prev_weak_counter = weak_counter_ptr->load(mo_acquire);
				while (prev_weak_counter > 0 && !weak_counter_ptr->compare_exchange_weak(prev_weak_counter, prev_weak_counter - 1, mo_release, mo_relaxed))
				{}

				if (prev_weak_counter == 1)
					_resource->destroy_self();
			}
		}

	public:

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

		virtual ~smart_ptr() {}

		virtual void reset() = 0;

		siz get_reference_count() const
		{
			atm_siz* reference_counter_ptr = get_reference_counter_ptr();
			return reference_counter_ptr ? reference_counter_ptr->load(mo_acquire) : 0;
		}

		siz get_weak_count() const
		{
			atm_siz* weak_counter_ptr = get_weak_counter_ptr();
			return weak_counter_ptr ? weak_counter_ptr->load(mo_acquire) : 0;
		}
	};

	template <typename T>
	class sptr : public smart_ptr<T>
	{
	private:
		template <typename U>
		friend class wptr;

		template <typename U>
		friend class sptr;

		using base = smart_ptr<T>;

		T* get_object_ptr() const
		{
			return _resource ? (T*)_resource->get_object_ptr() : nullptr;
		}

	public:

		sptr() : base() {}
			
		sptr(nptr): base() {}

		sptr(smart_ptr_resource_base* resource) : base(resource)
		{
			increment_reference_counter();
			increment_weak_counter();
		}

		sptr(const sptr<T>& other) : base(other)
		{
			increment_reference_counter();
			increment_weak_counter();
		}

		sptr(sptr<T>&& other) noexcept : base(::std::move(other)) {}

		template <typename U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, bl> = true>
		sptr(const sptr<U>& other): base(other)
		{
			increment_reference_counter();
			increment_weak_counter();
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
			_resource = other._resource;
			increment_reference_counter();
			increment_weak_counter();
			return *this;
		}

		sptr<T>& operator=(sptr<T>&& other) noexcept
		{
			reset();
			_resource = ::std::move(other._resource);
			other._resource = nullptr;
			return *this;
		}

		template <typename U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, bl> = true>
		sptr<T>& operator=(const sptr<U>& other)
		{
			reset();
			_resource = other._resource;
			increment_reference_counter();
			increment_weak_counter();
			return *this;
		}

		template <typename U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, bl> = true>
		sptr<T>& operator=(sptr<U>&& other) noexcept
		{
			reset();
			_resource = ::std::move(other._resource);
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
			decrement_reference_counter();
			decrement_weak_counter();
			_resource = nullptr;
		}
	};

	template <typename T>
	class wptr : public smart_ptr<T>
	{
	private:
		template <typename U>
		friend class wptr;

		using base = smart_ptr<T>;

	protected:
		virtual void increment_reference_counter() override {}

		virtual void decrement_reference_counter() override {}

	public:

		wptr() :base(nullptr) {}

		wptr(nptr) : base() {}

		wptr(const sptr<T>& other): base(other)
		{
			increment_weak_counter();
		}

		wptr(const wptr<T>& other) : base(other)
		{
			increment_weak_counter();
		}

		wptr(wptr<T>&& other) noexcept : base(::std::move(other)) {}

		template <typename U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, bl> = true>
		wptr(const sptr<U>& other) : base(other)
		{
			increment_weak_counter();
		}

		template <typename U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, bl> = true>
		wptr(const wptr<U>& other) : base(other)
		{
			increment_weak_counter();
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
			_resource = other._resource;
			increment_weak_counter();
			return *this;
		}

		wptr<T>& operator=(const wptr<T>& other)
		{
			reset();
			_resource = other._resource;
			increment_weak_counter();
			return *this;
		}

		wptr<T>& operator=(wptr<T>&& other) noexcept
		{
			reset();
			_resource = ::std::move(other._resource);
			other._resource = nullptr;
			return *this;
		}

		template <typename U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, bl> = true>
		wptr<T>& operator=(const sptr<U>& other)
		{
			reset();
			_resource = other._resource;
			increment_weak_counter();
			return *this;
		}

		template <typename U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, bl> = true>
		wptr<T>& operator=(const wptr<U>& other)
		{
			reset();
			_resource = other._resource;
			increment_weak_counter();
			return *this;
		}

		template <typename U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, bl> = true>
		wptr<T>& operator=(wptr<U>&& other) noexcept
		{
			reset();
			_resource = ::std::move(other._resource);
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
			decrement_weak_counter();
			_resource = nullptr;
		}

		bl is_expired() const
		{
			return get_reference_count() == 0;
		}

		sptr<T> get_sptr() const
		{
			return sptr<T>(_resource);
		}
	};
	
	template <typename T, typename R>
	struct smart_ptr_contiguous_block
	{
		NP_ENGINE_STATIC_ASSERT((::std::is_base_of_v<smart_ptr_resource_base, R>), "(D)estroyer must derive from smart_ptr_resource_base");

		using resource_block_type = SizedBlock<sizeof(R)>;
		using object_block_type = SizedBlock<sizeof(T)>;

		resource_block_type resource_block;
		object_block_type object_block;
	};

	template <typename T, typename... Args>
	constexpr uptr<T> create_uptr(Args&&... args)
	{
		using destroyer_type = typename uptr<T>::destroyer_type;
		TraitAllocator allocator;
		return uptr<T>(destroyer_type{}, mem::Create<T>(allocator, ::std::forward<Args>(args)...));
	}

	template <typename T, typename... Args>
	constexpr sptr<T> create_sptr(Allocator& allocator, Args&&... args)
	{
		using destroyer_type = smart_contiguous_destroyer<T>;
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

#endif /* NP_ENGINE_SMART_PTR_HPP */