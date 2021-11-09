//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/9/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Memory/TraitAllocator.hpp"

::np::memory::Allocator* ::np::memory::TraitAllocator::_allocator = ::np::memory::AddressOf(::np::memory::DefaultAllocator);