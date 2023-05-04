//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/19/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_DEVICE_RESOURCE_POOL_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_DEVICE_RESOURCE_POOL_HPP

#include "NP-Engine/Memory/Memory.hpp"

#include "Resource.hpp"

namespace np::gpu
{
	using DeviceResourcePool = mem::AccumulatingPool<Resource>; //TODO: could this be an accumulating pool??
}

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_DEVICE_RESOURCE_POOL_HPP */