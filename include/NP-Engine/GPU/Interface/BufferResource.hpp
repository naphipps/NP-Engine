//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_BUFFER_RESOURCE_HPP
#define NP_ENGINE_GPU_INTERFACE_BUFFER_RESOURCE_HPP

#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "Device.hpp"
#include "Resource.hpp"

namespace np::gpu
{
	class BufferResourceUsage : public ResourceUsage
	{
	public:
		constexpr static ui32 Read = BIT(0);
		constexpr static ui32 Write = BIT(1);

		constexpr static ui32 HostAccessible = BIT(2);
		constexpr static ui32 Sparse = BIT(3);
		constexpr static ui32 Dynamic = BIT(4);
		constexpr static ui32 Indirect = BIT(5); //TODO: I feel like we can make a better name for buffers that are used for cmds (like indirect dispatch)

		constexpr static ui32 Transfer = BIT(6);
		constexpr static ui32 Uniform = BIT(7);
		constexpr static ui32 Storage = BIT(8);
		constexpr static ui32 Texel = BIT(9);
		constexpr static ui32 Index = BIT(10);
		constexpr static ui32 Vertex = BIT(11);
		
		BufferResourceUsage(ui32 value) : ResourceUsage(value) {}
	};

	struct BufferResource : public Resource
	{
		static mem::sptr<BufferResource> Create(mem::sptr<Device> device, BufferResourceUsage usage, siz size, const con::vector<DeviceQueueFamily>& queue_families); //TODO: accept formats? memory flags?

		virtual ~BufferResource() = default;

		virtual mem::sptr<Device> GetDevice() const = 0;

		virtual ResourceType GetResourceType() const override
		{
			return ResourceType::Buffer;
		}

		virtual siz GetSize() const = 0;

		virtual con::vector<DeviceQueueFamily> GetDeviceQueueFamilies() const = 0;

		virtual bl Resize(siz size) = 0;

		virtual bl Assign(siz offset, const con::vector<ui8>& bytes) = 0;
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_BUFFER_RESOURCE_HPP */