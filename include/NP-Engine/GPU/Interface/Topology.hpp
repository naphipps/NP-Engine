//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_TOPOLOGY_HPP
#define NP_ENGINE_GPU_INTERFACE_TOPOLOGY_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::gpu
{
	//TODO: should this be moved to geom?

	class PrimitiveTopology : public Enum<ui32> //TODO: we might be fine to rename this to Topology? maybe one day
	{
	public:
		constexpr static ui32 Resettable = BIT(0); //use the relevant "_MAX" value to represent the reset value
		constexpr static ui32 Adjacency = BIT(1); //TODO: used with geometry shaders -- investigate

		constexpr static ui32 Point = BIT(2);
		constexpr static ui32 Line = BIT(3);
		constexpr static ui32 Triangle = BIT(4);
		constexpr static ui32 Patch = BIT(5); //TODO: used with tessellation -- investigate

		constexpr static ui32 List = BIT(6);
		constexpr static ui32 Strip = BIT(7);
		constexpr static ui32 Fan = BIT(8);

		PrimitiveTopology(ui32 value) :Enum<ui32>(value) {}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_TOPOLOGY_HPP */