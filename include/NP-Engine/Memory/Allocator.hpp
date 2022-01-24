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

namespace np::memory
{
	/*
		constructs the given type using the given args inside given block
	*/
	template <typename T, typename... Args>
	constexpr bl Construct(const Block& block, Args&&... args)
	{
		bl constructed = false;

		if (block.IsValid() && block.size >= sizeof(T))
		{
			new (block.ptr) T(::std::forward<Args>(args)...);
			constructed = true;
		}

		return constructed;
	}

	/*
		constructs the given type using the given args inside given block
	*/
	template <typename T>
	constexpr bl ConstructArray(const Block& block, siz size)
	{
		bl constructed = false;

		if (block.IsValid() && block.size >= sizeof(T))
		{
			new (block.ptr) T[size];
			constructed = true;
		}

		return constructed;
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
			{
				t[i].~T();
			}
			destructed = true;
		}

		return destructed;
	}

	/*
		the alignment our allocators will adhere to
	*/
	constexpr static siz ALIGNMENT = BIT(3);

	constexpr static siz CalcAlignedSize(const siz size)
	{
		return ((size + ALIGNMENT - 1) / ALIGNMENT) * ALIGNMENT;
	}

	class Allocator
	{
	public:
		virtual bl Contains(const Block& block) const = 0;

		virtual bl Contains(const void* ptr) const = 0;

		virtual Block Allocate(siz size) = 0;

		virtual bl Deallocate(Block& block) = 0;

		virtual bl Deallocate(void* ptr) = 0;
	};
} // namespace np::memory

#endif /* NP_ENGINE_ALLOCATOR_HPP */
