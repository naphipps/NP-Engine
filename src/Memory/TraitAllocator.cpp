//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/9/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Memory/TraitAllocator.hpp"

namespace np::mem
{
	c_allocator trait_allocator::_default_allocator;
	atm<allocator*> trait_allocator::_registered_allocator(address_of(trait_allocator::_default_allocator));
} // namespace np::mem