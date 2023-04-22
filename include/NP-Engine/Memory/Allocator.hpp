//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/16/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_ALLOCATOR_HPP
#define NP_ENGINE_ALLOCATOR_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Block.hpp"

namespace np::mem
{
	/*
		constructs the given type using the given args inside given block
	*/
	template <typename T, typename... Args>
	constexpr T* Construct(Block block, Args&&... args)
	{
		T* object = nullptr;
		if (block.IsValid() && block.size >= sizeof(T))
			object = new (block.ptr) T(::std::forward<Args>(args)...);

		return object;
	}

	/*
		constructs the given type using the given args inside given block
	*/
	template <typename T>
	constexpr T* ConstructArray(Block block, siz size)
	{
		T* array = nullptr;
		if (block.IsValid() && block.size >= (sizeof(T) * size))
			array = new (block.ptr) T[size];

		return array;
	}

	/*
		destructs the object given it's pointer
	*/
	template <typename T>
	constexpr bl Destruct(const T* t)
	{
		bl destructed = false;

		if (t != nullptr)
		{
			t->~T();
			destructed = true;
		}

		return destructed;
	}

	/*
		destructs the object given it's pointer
	*/
	template <typename T>
	constexpr bl DestructArray(const T* t, siz size)
	{
		bl destructed = false;

		if (t != nullptr)
		{
			for (siz i = 0; i < size; i++)
				t[i].~T();

			destructed = true;
		}

		return destructed;
	}

	class Allocator
	{
	public:
		constexpr static siz OVERHEAD_SIZE = 0; // TODO: a lot of allocators are not implementing this...

		virtual bl Contains(const Block& block) const = 0;

		virtual bl Contains(const void* ptr) const = 0;

		virtual Block Allocate(siz size) = 0;

		virtual Block Reallocate(Block& old_block, siz new_size) = 0;

		virtual Block Reallocate(void* old_ptr, siz new_size) = 0;

		virtual bl Deallocate(Block& block) = 0;

		virtual bl Deallocate(void* ptr) = 0;
	};

	template <typename T, typename... Args>
	T* Create(Allocator& allocator, Args&&... args)
	{
		return mem::Construct<T>(allocator.Allocate(sizeof(T)), ::std::forward<Args>(args)...);;
	}

	template <typename T>
	void Destroy(Allocator& allocator, T* ptr)
	{
		mem::Destruct<T>(ptr);
		allocator.Deallocate((void*)ptr);
	}
} // namespace np::mem

#endif /* NP_ENGINE_ALLOCATOR_HPP */
