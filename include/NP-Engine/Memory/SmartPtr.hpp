//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/29/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_SMART_PTR_HPP
#define NP_ENGINE_SMART_PTR_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "TraitAllocator.hpp"

#include <memory>
#include <type_traits>
#include <utility>

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
				NP_STATIC_ASSERT(0 < sizeof(T), "can't delete an incomplete type");
				memory::Destruct(ptr);
				DefaultTraitAllocator.Deallocate(ptr);
			}
		};
	} // namespace __detail

	template <class T>
	using uptr = ::std::unique_ptr<T, __detail::SmartPtrDeleter<T>>;

	template <typename T, typename... Args>
	constexpr uptr<T> CreateUptr(Args&&... args)
	{
		Block block = DefaultTraitAllocator.Allocate(sizeof(T));
		memory::Construct<T>(block, ::std::forward<Args>(args)...); // TODO: NP_ASSERT??
		return uptr<T>((T*)block.ptr);
	}

	template <typename T>
	using sptr = ::std::shared_ptr<T>;

	template <typename T, typename... Args>
	constexpr sptr<T> CreateSptr(Args&&... args)
	{
		Block block = DefaultTraitAllocator.Allocate(sizeof(T));
		memory::Construct<T>(block, ::std::forward<Args>(args)...); // TODO: NP_ASSERT??
		return sptr<T>((T*)block.ptr, __detail::SmartPtrDeleter<T>{});
	}
} // namespace np::memory

#endif /* NP_ENGINE_SMART_PTR_HPP */