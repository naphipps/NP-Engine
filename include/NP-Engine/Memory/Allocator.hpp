//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/16/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_ALLOCATOR_HPP
#define NP_ENGINE_MEM_ALLOCATOR_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Alignment.hpp"
#include "Block.hpp"
#include "MemoryFunctions.hpp"

namespace np::mem
{
	/*
		constructs the given type using the given args inside given block
	*/
	template <typename T, typename... Args,
		::std::enable_if_t<mem::is_parenthesis_constructible_v<T, Args...> || mem::is_list_constructible_v<T, Args...>, bl> = true>
	constexpr T* construct(block b, Args&&... args)
	{
		return b.is_valid() && b.size >= sizeof(T) ? ::new (b.ptr) T{ ::std::forward<Args>(args)... } : nullptr;
	}

	/*
		destructs the object given it's pointer
	*/
	template <typename T>
	constexpr bl destruct(const T* t)
	{
		bl destructed = false;
		if (t != nullptr)
		{
			t->~T();
			destructed = true;
		}
		return destructed;
	}

	class allocator
	{
	public:
		virtual ~allocator() = default;

		virtual bl contains(const block& b) = 0;

		virtual bl contains(const void* ptr) = 0;

		virtual block allocate(siz size, siz alignment) = 0;

		/*
			b is assumed to be the aligned block we return in allocate/reallocate calls
		*/
		virtual block reallocate(block& b, siz size, siz alignment) = 0;

		/*
			ptr is assumed to be the aligned block.ptr/begin() we return in allocate/reallocate calls
		*/
		virtual block reallocate(void* ptr, siz size, siz alignment) = 0; // TODO: considering removing this? everything either is able to extract it's block or does not use this, so I think it's fine

		virtual bl deallocate(block& b) = 0;

		virtual bl deallocate(void* ptr) = 0;
	};
	
	template <typename T, typename... Args,
		::std::enable_if_t<mem::is_parenthesis_constructible_v<T, Args...> || mem::is_list_constructible_v<T, Args...>, bl> = true>
	constexpr T * create(allocator& a, siz alignment, Args&&... args)
	{
		return mem::construct<T>(a.allocate(sizeof(T), alignment), ::std::forward<Args>(args)...);
	}

	template <typename T, typename... Args,
		::std::enable_if_t<mem::is_parenthesis_constructible_v<T, Args...> || mem::is_list_constructible_v<T, Args...>, bl> = true>
	constexpr T* create(allocator& a, Args&&... args)
	{
		return mem::create<T, Args...>(a, mem::DEFAULT_ALIGNMENT, ::std::forward<Args>(args)...);
	}

	template <typename T>
	constexpr void destroy(allocator& a, T* ptr)
	{
		mem::destruct<T>(ptr);
		a.deallocate(static_cast<void*>(ptr));
	}
} // namespace np::mem

#endif /* NP_ENGINE_MEM_ALLOCATOR_HPP */
