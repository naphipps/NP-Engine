//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/9/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Memory/TraitAllocator.hpp"

namespace np::memory
{
	CAllocator TraitAllocator::_default_allocator;
	atm<Allocator*> TraitAllocator::_registered_allocator(AddressOf(TraitAllocator::_default_allocator));
} // namespace np::memory