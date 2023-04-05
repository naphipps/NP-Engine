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

//TODO: sptr and wptr can inherit from SmartPtr that has the _resource, common methods, etc

namespace np::mem
{
	struct SmartPtrResourceBase
	{
		atm_siz referenceCounter = 0;
		atm_siz weakCounter = 0;

		virtual ~SmartPtrResourceBase() = default;

		virtual void DestroyObject() = 0;
		virtual void DestroySelf() = 0;
		virtual void* GetObjectPtr() const = 0;
	};

	template <typename T>
	struct SmartPtrDestroyer
	{
		virtual void DestructObject(T* ptr)
		{
			mem::Destruct<T>(ptr);
		}

		virtual void DeallocateObject(T* ptr)
		{
			TraitAllocator allocator;
			allocator.Deallocate(ptr);
		}

		virtual void DestructResource(SmartPtrResourceBase* ptr)
		{
			mem::Destruct<SmartPtrResourceBase>(ptr);
		}

		virtual void DeallocateResource(SmartPtrResourceBase* ptr)
		{
			TraitAllocator allocator;
			allocator.Deallocate(ptr);
		}
	};

	template <typename T, typename D = SmartPtrDestroyer<T>>
	class SmartPtrResource : public SmartPtrResourceBase
	{
	private:
		NP_ENGINE_STATIC_ASSERT(::std::is_copy_constructible_v<D>, "(D)estroyer must be copy constructible");
		NP_ENGINE_STATIC_ASSERT(::std::is_move_constructible_v<D>, "(D)estroyer must be move constructible");
		NP_ENGINE_STATIC_ASSERT((::std::is_base_of_v<SmartPtrDestroyer<T>, D>), "(D)estroyer must derive from SmartPtrDestroyer<T>");

	public:
		using Destroyer = D;

	protected:
		Destroyer _destroyer;
		T* _object;

	public:
		SmartPtrResource(Destroyer destroyer, T* object) : _destroyer(destroyer), _object(object) {}

		virtual void DestroyObject() override
		{
			_destroyer.DestructObject(_object);
			_destroyer.DeallocateObject(_object);
			_object = nullptr;
		}

		virtual void DestroySelf() override
		{
			_destroyer.DestructResource(this);
			_destroyer.DeallocateResource(this);
		}

		void* GetObjectPtr() const override
		{
			return _object;
		}
	};

	template <typename T>
	class SmartAllocatorDestroyer : public SmartPtrDestroyer<T>
	{
	protected:
		Allocator& _allocator;

	public:
		SmartAllocatorDestroyer(Allocator& allocator) : _allocator(allocator) {}

		SmartAllocatorDestroyer(const SmartAllocatorDestroyer& other) : _allocator(other._allocator) {}

		SmartAllocatorDestroyer(SmartAllocatorDestroyer&& other) noexcept : _allocator(other._allocator) {}

		virtual void DeallocateObject(T* ptr) override
		{
			_allocator.Deallocate(ptr);
		}

		virtual void DeallocateResource(SmartPtrResourceBase* ptr) override
		{
			_allocator.Deallocate(ptr);
		}
	};

	template <typename T>
	class SmartContiguousDestroyer : public SmartAllocatorDestroyer<T>
	{
	private:
		using base = SmartAllocatorDestroyer<T>;

	public:
		SmartContiguousDestroyer(Allocator& allocator) : base(allocator) {}

		SmartContiguousDestroyer(const SmartContiguousDestroyer& other) : base(other._allocator) {}

		SmartContiguousDestroyer(SmartContiguousDestroyer&& other) noexcept : base(other._allocator) {}

		virtual void DeallocateObject(T* ptr) override {}

		virtual void DeallocateResource(SmartPtrResourceBase* ptr) override
		{
			_allocator.Deallocate(ptr);
		}
	};

	template <typename T>
	class uptr
	{
	public:
		using Destroyer = SmartPtrDestroyer<T>;

	private:
		T* _object;
		Destroyer _destroyer;

	public:

		uptr() : _object(nullptr), _destroyer() {}

		uptr(nptr): uptr() {}

		template <typename... Args>
		uptr(Destroyer destroyer, T* object): _object(object), _destroyer(destroyer) {}

		uptr(uptr<T>&& other) noexcept : _object(::std::move(other._object)), _destroyer(::std::move(other._destroyer)) {}
		
		~uptr()
		{
			Reset();
		}

		uptr<T>& operator=(uptr<T>&& other) noexcept
		{
			Reset();
			_object = ::std::move(other._object);
			other._object = nullptr;
			return *this;
		}

		void Reset()
		{
			if (_object)
			{
				_destroyer.DestructObject(_object);
				_destroyer.DeallocateObject(_object);
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
	class wptr;

	template <typename T>
	class sptr
	{
	private:
		template <typename T2>
		friend class wptr;

		template <typename T2>
		friend class sptr;

		SmartPtrResourceBase* _resource;
		
		atm_siz* GetReferenceCounterPtr() const
		{
			return _resource ? mem::AddressOf(_resource->referenceCounter) : nullptr;
		}

		atm_siz* GetWeakCounterPtr() const
		{
			return _resource ? mem::AddressOf(_resource->weakCounter) : nullptr;
		}

		T* GetObjectPtr() const
		{
			return _resource ? (T*)_resource->GetObjectPtr() : nullptr;
		}

	public:

		sptr(): _resource(nullptr) {}
			
		sptr(nptr): sptr() {}

		template <typename... Args>
		sptr(SmartPtrResourceBase* resource) : _resource(resource)
		{
			if (_resource)
			{
				(*GetReferenceCounterPtr())++;
				(*GetWeakCounterPtr())++;
			}
		}

		sptr(const sptr<T>& other) : _resource(other._resource)
		{
			if (_resource)
			{
				(*GetReferenceCounterPtr())++;
				(*GetWeakCounterPtr())++;
			}
		}

		sptr(sptr<T>&& other) noexcept : _resource(::std::move(other._resource))
		{
			other._resource = nullptr;
		}

		template <typename T2, ::std::enable_if_t<::std::is_convertible_v<T2*, T*>, bl> = true>
		sptr(const sptr<T2>& other): _resource(other._resource)
		{
			if (_resource)
			{
				(*GetReferenceCounterPtr())++;
				(*GetWeakCounterPtr())++;
			}
		}

		template <typename T2, ::std::enable_if_t<::std::is_convertible_v<T2*, T*>, bl> = true>
		sptr(sptr<T2>&& other) noexcept : _resource(::std::move(other._resource))
		{
			other._resource = nullptr;
		}

		~sptr()
		{
			Reset();
		}

		sptr<T>& operator=(const sptr<T>& other)
		{
			Reset();
			_resource = other._resource;
			if (_resource)
			{
				(*GetReferenceCounterPtr())++;
				(*GetWeakCounterPtr())++;
			}

			return *this;
		}

		sptr<T>& operator=(sptr<T>&& other) noexcept
		{
			Reset();
			_resource = ::std::move(other._resource);
			other._resource = nullptr;
			return *this;
		}

		template <typename T2, ::std::enable_if_t<::std::is_convertible_v<T2*, T*>, bl> = true>
		sptr<T>& operator=(const sptr<T2>& other)
		{
			Reset();
			_resource = other._resource;
			if (_resource)
			{
				(*GetReferenceCounterPtr())++;
				(*GetWeakCounterPtr())++;
			}

			return *this;
		}

		template <typename T2, ::std::enable_if_t<::std::is_convertible_v<T2*, T*>, bl> = true>
		sptr<T>& operator=(sptr<T2>&& other) noexcept
		{
			Reset();
			_resource = ::std::move(other._resource);
			other._resource = nullptr;
			return *this;
		}

		sptr<T>& operator=(nptr)
		{
			Reset();
			return *this;
		}

		T& operator*() const
		{
			return *GetObjectPtr();
		}

		T* operator->() const
		{
			return GetObjectPtr();
		}

		bl operator==(const sptr<T>& other) const
		{
			return GetObjectPtr() == other.GetObjectPtr();
		}

		operator bl() const
		{
			return GetObjectPtr();
		}

		void Reset()
		{
			atm_siz* reference_counter_ptr = GetReferenceCounterPtr();
			if (reference_counter_ptr)
			{
				siz prev_reference_count = reference_counter_ptr->load(mo_acquire);
				while (prev_reference_count > 0 && !reference_counter_ptr->compare_exchange_weak(prev_reference_count, prev_reference_count - 1, mo_release, mo_relaxed))
				{}

				if (prev_reference_count == 1)
					_resource->DestroyObject();
			}

			atm_siz* weak_counter_ptr = GetWeakCounterPtr();
			if (weak_counter_ptr)
			{
				siz prev_weak_counter = weak_counter_ptr->load(mo_acquire);
				while (prev_weak_counter > 0 && !weak_counter_ptr->compare_exchange_weak(prev_weak_counter, prev_weak_counter - 1, mo_release, mo_relaxed))
				{}

				if (prev_weak_counter == 1)
					_resource->DestroySelf();
			}

			_resource = nullptr;
		}

		siz GetReferenceCount() const
		{
			atm_siz* reference_counter = GetReferenceCounterPtr();
			return reference_counter ? reference_counter->load(mo_acquire) : 0;
		}

		siz GetWeakCount() const
		{
			atm_siz* weak_counter = GetWeakCounterPtr();
			return weak_counter ? weak_counter->load(mo_acquire) : 0;
		}
	};

	template <typename T>
	class wptr
	{
	private:
		SmartPtrResourceBase* _resource;

		atm_siz* GetReferenceCounterPtr() const
		{
			return _resource ? mem::AddressOf(_resource->referenceCounter) : nullptr;
		}

		atm_siz* GetWeakCounterPtr() const
		{
			return _resource ? mem::AddressOf(_resource->weakCounter) : nullptr;
		}

	public:

		wptr() :_resource(nullptr) {}

		wptr(nptr) : wptr() {}

		wptr(const sptr<T>& other): _resource(other._resource)
		{
			if (_resource)
				(*GetWeakCounterPtr())++;
		}

		wptr(const wptr<T>& other) : _resource(other._resource)
		{
			if (_resource)
				(*GetWeakCounterPtr())++;
		}

		wptr(wptr<T>&& other) noexcept : _resource(::std::move(other._resource))
		{
			other._resource = nullptr;
		}

		template <typename T2, ::std::enable_if_t<::std::is_convertible_v<T2*, T*>, bl> = true>
		wptr(const sptr<T2>& other) : _resource(other._resource)
		{
			if (_resource)
				(*GetWeakCounterPtr())++;
		}

		template <typename T2, ::std::enable_if_t<::std::is_convertible_v<T2*, T*>, bl> = true>
		wptr(const wptr<T2>& other) : _resource(other._resource)
		{
			if (_resource)
				(*GetWeakCounterPtr())++;
		}

		template <typename T2, ::std::enable_if_t<::std::is_convertible_v<T2*, T*>, bl> = true>
		wptr(wptr<T2>&& other) noexcept : _resource(::std::move(other._resource))
		{
			other._resource = nullptr;
		}

		~wptr()
		{
			Reset();
		}

		wptr<T>& operator=(const sptr<T>& other)
		{
			Reset();
			_resource = other._resource;
			if (_resource)
				(*GetWeakCounterPtr())++;

			return *this;
		}

		wptr<T>& operator=(const wptr<T>& other)
		{
			Reset();
			_resource = other._resource;
			if (_resource)
				(*GetWeakCounterPtr())++;

			return *this;
		}

		wptr<T>& operator=(wptr<T>&& other) noexcept
		{
			Reset();
			_resource = ::std::move(other._resource);
			other._resource = nullptr;
			return *this;
		}

		template <typename T2, ::std::enable_if_t<::std::is_convertible_v<T2*, T*>, bl> = true>
		wptr<T>& operator=(const sptr<T2>& other)
		{
			Reset();
			_resource = other._resource;
			if (_resource)
				(*GetWeakCounterPtr())++;

			return *this;
		}

		template <typename T2, ::std::enable_if_t<::std::is_convertible_v<T2*, T*>, bl> = true>
		wptr<T>& operator=(const wptr<T2>& other)
		{
			Reset();
			_resource = other._resource;
			if (_resource)
				(*GetWeakCounterPtr())++;

			return *this;
		}

		template <typename T2, ::std::enable_if_t<::std::is_convertible_v<T2*, T*>, bl> = true>
		wptr<T>& operator=(wptr<T2>&& other) noexcept
		{
			Reset();
			_resource = ::std::move(other._resource);
			other._resource = nullptr;
			return *this;
		}

		wptr<T>& operator=(nptr)
		{
			Reset();
			return *this;
		}

		void Reset()
		{
			atm_siz* weak_counter_ptr = GetWeakCounterPtr();
			if (weak_counter_ptr)
			{
				siz prev_weak_counter = weak_counter_ptr->load(mo_acquire);
				while (prev_weak_counter > 0 && !weak_counter_ptr->compare_exchange_weak(prev_weak_counter, prev_weak_counter - 1, mo_release, mo_relaxed))
				{}

				if (prev_weak_counter == 1)
					_resource->DestroySelf();
			}

			_resource = nullptr;
		}

		bl IsExpired() const
		{
			return GetReferenceCount() == 0;
		}

		siz GetReferenceCount() const
		{
			atm_siz* reference_counter = GetReferenceCounterPtr();
			return reference_counter ? reference_counter->load(mo_acquire) : 0;
		}

		siz GetWeakCount() const
		{
			atm_siz* weak_counter = GetWeakCounterPtr();
			return weak_counter ? weak_counter->load(mo_acquire) : 0;
		}

		sptr<T> GetSptr() const
		{
			sptr<T> ptr;

			if (_resource)
			{
				(*GetReferenceCounterPtr())++;
				(*GetWeakCounterPtr())++;

				ptr._resource = _resource;
			}

			return ptr;
		}
	};
	
	template <typename T>
	struct SmartPtrBlock
	{
		constexpr static siz SIZE = CalcAlignedSize(sizeof(T));

		ui8 allocation[SIZE];

		operator Block() const
		{
			return { (void*)allocation, SIZE };
		}
	};

	template <typename T, typename R>
	struct SmartPtrContiguousBlock
	{
		NP_ENGINE_STATIC_ASSERT((::std::is_base_of_v<SmartPtrResourceBase, R>), "(D)estroyer must derive from SmartPtrResourceBase");

		using ResourceBlockType = SmartPtrBlock<R>;
		using ObjectBlockType = SmartPtrBlock<T>;

		ResourceBlockType resourceBlock;
		ObjectBlockType objectBlock;
	};

	template <typename T, typename... Args>
	constexpr uptr<T> CreateUptr(Args&&... args)
	{
		using DestroyerType = typename uptr<T>::Destroyer;
		TraitAllocator allocator;
		return uptr<T>(DestroyerType{}, mem::Create<T>(allocator, ::std::forward<Args>(args)...));
	}

	template <typename T, typename... Args>
	constexpr sptr<T> CreateSptr(Allocator& allocator, Args&&... args)
	{
		using DestroyerType = SmartContiguousDestroyer<T>;
		using ResourceType = SmartPtrResource<T, DestroyerType>;
		using ContiguousBlockType = SmartPtrContiguousBlock<T, ResourceType>;
		
		ResourceType* resource = nullptr;
		ContiguousBlockType* contiguous_block = mem::Create<ContiguousBlockType>(allocator);
		if (contiguous_block)
		{
			T* object = mem::Construct<T>(contiguous_block->objectBlock, ::std::forward<Args>(args)...);
			resource = mem::Construct<ResourceType>(contiguous_block->resourceBlock, DestroyerType(allocator), object);
		}
		return sptr<T>(resource);
	}
} // namespace np::mem

#endif /* NP_ENGINE_SMART_PTR_HPP */