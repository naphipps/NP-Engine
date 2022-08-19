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

namespace np::memory
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
				NP_ENGINE_STATIC_ASSERT(0 < sizeof(T), "can't delete an incomplete type"); // TODO: what is this?
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
		TraitAllocator allocator;
		Block block = allocator.Allocate(sizeof(T));
		NP_ENGINE_ASSERT((::std::is_constructible_v<T, Args...>), "T must be constructible with the given args.");
		bl constructed = memory::Construct<T>(block, ::std::forward<Args>(args)...);
		NP_ENGINE_ASSERT(constructed, "We require a successful construction here.");
		return uptr<T>(static_cast<T*>(block.ptr));
	}

	template <typename T>
	using sptr = ::std::shared_ptr<T>;

	template <typename T, typename... Args>
	constexpr sptr<T> CreateSptr(Args&&... args)
	{
		TraitAllocator allocator;
		Block block = allocator.Allocate(sizeof(T));
		NP_ENGINE_ASSERT((::std::is_constructible_v<T, Args...>), "T must be constructible with the given args.");
		bl constructed = memory::Construct<T>(block, ::std::forward<Args>(args)...);
		NP_ENGINE_ASSERT(constructed, "We require a successful construction here.");
		return sptr<T>(static_cast<T*>(block.ptr), __detail::SmartPtrDeleter<T>{});
	}
} // namespace np::memory

#endif /* NP_ENGINE_SMART_PTR_HPP */