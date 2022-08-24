//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/29/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_SMART_PTR_HPP
#define NP_ENGINE_SMART_PTR_HPP

#include <memory>
#include <type_traits>
#include <utility>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "TraitAllocator.hpp"

namespace np::mem
{
	namespace __detail
	{
		template <class T>
		struct SmartPtrDeleter : public ::std::default_delete<T>
		{
			constexpr SmartPtrDeleter() noexcept = default;

			template <class U, ::std::enable_if_t<::std::is_convertible_v<U*, T*>, i32> = 0>
			SmartPtrDeleter(const SmartPtrDeleter<U>&) noexcept
			{}

			void operator()(T* ptr) const noexcept
			{
				NP_ENGINE_STATIC_ASSERT(0 < sizeof(T), "can't delete an incomplete type");
				TraitAllocator allocator;
				Destroy<T>(allocator, ptr);
			}
		};
	} // namespace __detail

	template <class T>
	using uptr = ::std::unique_ptr<T, __detail::SmartPtrDeleter<T>>;

	template <typename T, typename... Args>
	constexpr uptr<T> CreateUptr(Args&&... args)
	{
		NP_ENGINE_ASSERT((::std::is_constructible_v<T, Args...>), "T must be constructible with the given args.");
		TraitAllocator allocator;
		T* ptr = Create<T>(allocator, ::std::forward<Args>(args)...);
		NP_ENGINE_ASSERT(ptr, "We require a successful construction here.");
		return uptr<T>(ptr);
	}

	template <typename T>
	using sptr = ::std::shared_ptr<T>;

	template <typename T, typename... Args>
	constexpr sptr<T> CreateSptr(Args&&... args)
	{
		NP_ENGINE_ASSERT((::std::is_constructible_v<T, Args...>), "T must be constructible with the given args.");
		TraitAllocator allocator;
		Block block = allocator.Allocate(sizeof(T));
		T* ptr = Create<T>(allocator, ::std::forward<Args>(args)...);
		NP_ENGINE_ASSERT(ptr, "We require a successful construction here.");
		return sptr<T>(ptr, __detail::SmartPtrDeleter<T>{});
	}
} // namespace np::mem

#endif /* NP_ENGINE_SMART_PTR_HPP */