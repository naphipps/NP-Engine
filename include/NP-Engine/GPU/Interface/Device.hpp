//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/12/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_DEVICE_HPP
#define NP_ENGINE_GPU_INTERFACE_DEVICE_HPP

#ifndef NP_ENGINE_GPU_INTERFACE_DEVICE_RESOURCE_POOL_DEFAULT_SIZE //TODO: what is this?
	#define NP_ENGINE_GPU_INTERFACE_DEVICE_POOL_RESOURCE_DEFAULT_SIZE 500
#endif

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Uid/Uid.hpp"

#include "PresentTarget.hpp"
#include "Fence.hpp"
#include "Semaphore.hpp"
#include "PipelineCache.hpp"

/*
	TODO: add RenderableShape -- lines, circulars, polygons
		- support different ways to draw these (like GL_LINE, GL_LINES, etc)
		- <http://www.dgp.toronto.edu/~ah/csc418/fall_2001/tut/ogl_draw.html>
		- <https://www.khronos.org/opengl/wiki/Primitive> their vertex convetions are nice
*/

namespace np::gpu
{
	class DeviceUsage : public enm_ui32
	{
	public:
		constexpr static ui32 Compute = BIT(0);
		constexpr static ui32 Graphics = BIT(1);
		constexpr static ui32 Present = BIT(2);

		constexpr static ui32 RayTrace = BIT(4); //TODO: investigate

		//constexpr static ui32 Transfer = BIT(0); //TODO: shall we support this?

		constexpr static ui32 MultiDrawIndirect = BIT(5); //TODO: investigate -- rename to MultiDraw?
		constexpr static ui32 Tessellation = BIT(6);

		DeviceUsage(ui32 value): enm_ui32(value) {}
	};

	class DeviceQueueUsage : public enm_ui32
	{
	public:
		constexpr static ui32 Present = BIT(0);
		constexpr static ui32 Graphics = BIT(1);
		constexpr static ui32 Compute = BIT(2);
		constexpr static ui32 RayTrace = BIT(4);

		//constexpr static ui32 Transfer = BIT(0); //TODO: shall we support this?

		DeviceQueueUsage(ui32 value): enm_ui32(value) {}
	};

	struct DeviceQueueFamily
	{
		siz index = SIZ_MAX;
		siz count = 0;
		DeviceQueueUsage usage = DeviceQueueUsage::None;
	};

	struct Device : public DetailObject
	{
		static mem::sptr<Device> Create(mem::sptr<DetailInstance> instance, DeviceUsage usage,
										mem::sptr<PresentTarget> target = nullptr);

		virtual ~Device() = default;

		virtual mem::sptr<DetailInstance> GetDetailInstance() const = 0;

		virtual mem::sptr<PresentTarget> GetPresentTarget() const = 0;

		virtual con::vector<DeviceQueueFamily> GetDeviceQueueFamilies() const = 0;

		virtual mem::sptr<PipelineCache> GetPipelineCache() const = 0;

		virtual mem::sptr<Fence> CreateFence() = 0;

		virtual mem::sptr<Semaphore> CreateSemaphore() = 0;

		virtual void WaitUntilIdle() const = 0;

		//virtual mem::sptr<PipelineCache> CreatePipelineCache(str filename) const = 0; //TODO: how do we want to do this?
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_DEVICE_HPP */