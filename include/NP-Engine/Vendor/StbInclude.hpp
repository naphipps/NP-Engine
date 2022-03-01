//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/18/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VENDOR_STB_INCLUDE_HPP
#define NP_ENGINE_VENDOR_STB_INCLUDE_HPP

#include <string>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#define STBI_ASSERT(expression) \
	NP_ASSERT(expression, ::std::string("STB encounted an asserted issue here: ") + ::std::string(NP_FUNCTION))

#define STBI_MALLOC(size) ::np::memory::DefaultTraitAllocator.Allocate(size).ptr
#define STBI_FREE(ptr) ::np::memory::DefaultTraitAllocator.Deallocate(ptr)

namespace np::memory::__detail
{
	static void* StbRealloc(void* ptr, siz size)
	{
		STBI_FREE(ptr);
		return STBI_MALLOC(size);
	}
} // namespace np::memory::__detail

#define STBI_REALLOC(ptr, size) ::np::memory::__detail::StbRealloc(ptr, size)

#define STBIW_ASSERT(expression) STBI_ASSERT(expression)
#define STBIW_MALLOC(size) STBI_MALLOC(size)
#define STBIW_FREE(ptr) STBI_FREE(ptr)
#define STBIW_REALLOC(ptr, size) STBI_REALLOC(ptr, size)

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image.h>
#include <stb_image_write.h>

#endif /* NP_ENGINE_VENDOR_STB_INCLUDE_HPP */
