//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/10/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VENDOR_RAPIDJSON_INCLUDE_HPP
#define NP_ENGINE_VENDOR_RAPIDJSON_INCLUDE_HPP

#include <utility>
#include <string>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#define RAPIDJSON_ASSERT(expression) \
	NP_ENGINE_ASSERT(expression, ::std::string("RAPIDJSON encounted an asserted issue here: ") + ::std::string(NP_FUNCTION))

#define RAPIDJSON_MALLOC(size) ::np::memory::DefaultTraitAllocator.Allocate(size).ptr
#define RAPIDJSON_FREE(ptr) ::np::memory::DefaultTraitAllocator.Deallocate(ptr)

namespace np::memory::__detail
{
	static void* RapidJsonRealloc(void* ptr, siz size)
	{
		RAPIDJSON_FREE(ptr);
		return RAPIDJSON_MALLOC(size);
	}

	template <typename T, typename... Args>
	static T* RapidJsonNew(Args&&... args)
	{
		Block block = DefaultTraitAllocator.Allocate(sizeof(T));
		bl constructed = Construct<T>(block, ::std::forward<Args>(args)...);
		RAPIDJSON_ASSERT(constructed);
		return (T*)block.ptr;
	}

	template <typename T>
	static void RapidJsonDelete(T* ptr)
	{
		bl destructed = Destruct<T>(ptr);
		RAPIDJSON_ASSERT(destructed);
		bl deallocated = DefaultTraitAllocator.Deallocate(ptr);
		RAPIDJSON_ASSERT(deallocated);
	}
} // namespace np::memory::__detail

#define RAPIDJSON_REALLOC(ptr, size) ::np::memory::__detail::RapidJsonRealloc(ptr, size)
#define RAPIDJSON_NEW(TypeName) ::np::memory::__detail::RapidJsonNew<TypeName>
#define RAPIDJSON_DELETE(ptr) ::np::memory::__detail::RapidJsonDelete(ptr)

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#endif /* NP_ENGINE_VENDOR_RAPIDJSON_INCLUDE_HPP */