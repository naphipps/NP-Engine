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
	template <typename T>
	class UptrDeleter : public ::std::default_delete<T>
	{
	public:
		using AllocatorType = TraitAllocator;

		void operator()(T* ptr) const noexcept
		{
			AllocatorType allocator;
			Destroy<T>(allocator, ptr);
		}
	};

	template <class T>
	using uptr = ::std::unique_ptr<T, UptrDeleter<T>>;

	template <typename T, typename... Args>
	constexpr uptr<T> CreateUptr(Args&&... args)
	{
		NP_ENGINE_ASSERT((::std::is_constructible_v<T, Args...>), "T must be constructible with the given args.");
		typename uptr<T>::deleter_type::AllocatorType allocator;
		return uptr<T>(Create<T>(allocator, ::std::forward<Args>(args)...));
	}

	template <typename T>
	class SptrDeleter : public ::std::default_delete<T>
	{
	private:
		Allocator& _allocator;

	public:
		SptrDeleter(Allocator& allocator) : _allocator(allocator) {}

		SptrDeleter(const SptrDeleter& other) : _allocator(other._allocator) {}

		SptrDeleter(SptrDeleter&& other) noexcept : _allocator(other._allocator) {}

		void operator()(T* ptr) const noexcept
		{
			Destroy<T>(_allocator, ptr);
		}
	};

	template <typename T>
	using sptr = ::std::shared_ptr<T>;

	template <typename T, typename... Args>
	constexpr sptr<T> CreateSptr(Allocator& allocator, Args&&... args)
	{
		NP_ENGINE_ASSERT((::std::is_constructible_v<T, Args...>), "T must be constructible with the given args.");
		return sptr<T>(Create<T>(allocator, ::std::forward<Args>(args)...), SptrDeleter<T>(allocator));
	}

	template <typename T>
	using wptr = ::std::weak_ptr<T>;
} // namespace np::mem

#endif /* NP_ENGINE_SMART_PTR_HPP */