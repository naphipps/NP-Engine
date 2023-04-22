//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/19/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_FNV_HPP
#define NP_ENGINE_FNV_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

/*
	Hashing is a pretty big problem, and I think hash_append is the answer... But its too much work for now :P
	<https://github.com/HowardHinnant/hash_append>
*/

namespace np::mat
{
	/*
		Glenn Fowler, Landon Curt Noll, and Kiem-Phong Vo.
		fnv-1a algorithm
		<http://www.isthe.com/chongo/tech/comp/fnv/index.html#FNV-1a>
	*/
	
	static inline ui32 HashFnv1aUi32(const void* src, siz byte_count, ui32 hash = (ui32)0x811c9dc5)
	{
		const ui32 prime = 0x01000193;
		ui8* it = (ui8*)src;
		const ui8* end = it + byte_count;

		for (; it != end; it++)
		{
			hash ^= (ui32)(*it);
			hash *= prime; //TODO: there are GCC optimizations available
		}

		return hash;
	}

	static inline ui64 HashFnv1aUi64(const void* src, siz byte_count, ui64 hash = (ui64)0xcbf29ce484222325ULL)
	{
		const ui64 prime = 0x100000001b3ULL;
		ui8* it = (ui8*)src;
		const ui8* end = it + byte_count;

		for (; it != end; it++)
		{
			hash ^= (ui64)(*it);
			hash *= prime; //TODO: there are GCC optimizations available
		}

		return hash;
	}
}

#endif /* NP_ENGINE_FNV_HPP */