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

// TODO: putting curly braces around the NP_ENGINE_ASSERT below will prevent rapidjson from complaining about empty body
// TODO: ^ but I think we ought to try to figure out how to just ignore that warning

#define RAPIDJSON_ASSERT(expression) \
	NP_ENGINE_ASSERT(expression, \
					 ::std::string("RAPIDJSON encounted an asserted issue here: ") + ::std::string(NP_ENGINE_FUNCTION))

namespace np::mem::__detail
{
	static void* RapidJsonMalloc(siz size)
	{
		void* result = TraitAllocator::malloc(size);
		RAPIDJSON_ASSERT(result);
		return result;
	}

	static void RapidJsonFree(void* ptr)
	{
		if (ptr)
			TraitAllocator::free(ptr);
	}

	static void* RapidJsonRealloc(void* old_ptr, siz new_size)
	{
		void* result = nullptr;

		if (old_ptr)
			result = TraitAllocator::realloc(old_ptr, new_size);
		else
			result = TraitAllocator::malloc(new_size);

		RAPIDJSON_ASSERT(result);
		return result;
	}

	template <typename T, typename... Args>
	static T* RapidJsonNew(Args&&... args)
	{
		siz size = sizeof(T);
		Block block{RapidJsonMalloc(size), size};
		bl constructed = Construct<T>(block, ::std::forward<Args>(args)...);
		RAPIDJSON_ASSERT(constructed);
		return (T*)block.ptr;
	}

	template <typename T>
	static void RapidJsonDelete(T* ptr)
	{
		if (ptr)
		{
			bl destructed = Destruct<T>(ptr);
			RAPIDJSON_ASSERT(destructed);
			RapidJsonFree(ptr);
		}
	}
} // namespace np::mem::__detail

#define RAPIDJSON_MALLOC(size) ::np::mem::__detail::RapidJsonMalloc(size)
#define RAPIDJSON_FREE(ptr) ::np::mem::__detail::RapidJsonFree(ptr)
#define RAPIDJSON_REALLOC(ptr, size) ::np::mem::__detail::RapidJsonRealloc(ptr, size)
#define RAPIDJSON_NEW(TypeName) ::np::mem::__detail::RapidJsonNew<TypeName>
#define RAPIDJSON_DELETE(ptr) ::np::mem::__detail::RapidJsonDelete(ptr)

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#endif /* NP_ENGINE_VENDOR_RAPIDJSON_INCLUDE_HPP */