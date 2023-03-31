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

	/*
		the alignment our allocators will adhere to
	*/
	constexpr static siz ALIGNMENT = BIT(3);
	constexpr static siz ALIGNMENT_MINUS_ONE = (ALIGNMENT - 1);

	constexpr static siz CalcAlignedSize(const siz size)
	{
		return ((size + ALIGNMENT - 1) / ALIGNMENT) * ALIGNMENT;
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
		Block block = allocator.Allocate(sizeof(T));
		T* object = mem::Construct<T>(block, ::std::forward<Args>(args)...);
		NP_ENGINE_ASSERT(object, "We require object of T to be constructed.");
		return object;
	}

	template <typename T>
	void Destroy(Allocator& allocator, T* ptr)
	{
		bl destructed = mem::Destruct<T>(ptr);
		bl deallocated = allocator.Deallocate((void*)ptr);
		NP_ENGINE_ASSERT(destructed && deallocated, "We required object of T to be destructed and deallocated.");
	}
} // namespace np::mem

#endif /* NP_ENGINE_ALLOCATOR_HPP */
