//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/9/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Memory/TraitAllocator.hpp"

namespace np::memory
{
	Allocator* TraitAllocator::_allocator = AddressOf(DefaultAllocator);
}