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
	TODO: the latest design is the following:

fps_model;
minimap_model;

fps_id
minimap_id

visible;

device;

fps_scene(device);
minimap_scene(device);

device.register(fps_id, fps_model) //creates rendering data for fps_model
device.register(minimap_id, minimap_model) //creates rendering data for minimap_model

//^ I think rendering data should be saved in the device as a sptr, so that when a frame packet uses it, they also use a sptr
//	^ this means that device.unregister(id) can remove device's sptr, so whenever the frame packet it done with it, then it will
destroy itself then
//		^ this only works if the sptr owns the rendering data

device.update(minimap_id) //updates the rendering data for minimap_model //api might not read "update"

fps_scene.register(visible, fps_id)
minimap_scene.register(visible, minimap_id)

fps_scene.render() //if visible is inside camera, then render fps_id
minimap_scene.render() //^ same but minimap_id

minimap_scene.unregister(visible) //minimap_id still exists
minimap_scene.render() // minimap_model will not be rendered
minimap_scene.register(visible, minimap_id) //minimap_model will be rendered
minimap_scene.unregister(visible)
device.unregister(minimap_id) //minimap_model rendering data will be marked to be destoryed
minimap_scene.render() // minimap_model will not be rendered
	* wait some time to ensure rendering data is destroyed *
minimap_scene.register(visible, minimap_id) // this should succeed whether or not minimap_id is associated with actual rendering
data minimap_scene.render() //every id will submitted to a job for visible-calculation-then-command-list-submission, but before
that, the job will check if "device.HasResource(id)" ^ since device.HasResource(minimap_id) would be false, then that job would
resolve there ^ this is more strain on the scene and job system, but enables us to reserve a spot before we have rendering data
on the device

*/

/*
	TODO: add RenderableShape -- lines, circulars, polygons
		- support different ways to draw these (like GL_LINE, GL_LINES, etc)
		- <http://www.dgp.toronto.edu/~ah/csc418/fall_2001/tut/ogl_draw.html>
		- <https://www.khronos.org/opengl/wiki/Primitive> their vertex convetions are nice
*/

namespace np::gpu
{
	class DeviceUsage : public Enum<ui32>
	{
	public:
		constexpr static ui32 Compute = BIT(0);
		constexpr static ui32 Graphics = BIT(1);
		constexpr static ui32 Present = BIT(2);

		constexpr static ui32 RayTrace = BIT(4); //TODO: investigate

		//constexpr static ui32 Transfer = BIT(0); //TODO: shall we support this?

		constexpr static ui32 MultiDrawIndirect = BIT(5); //TODO: investigate -- rename to MultiDraw?
		constexpr static ui32 Tessellation = BIT(6);

		DeviceUsage(ui32 value) : Enum<ui32>(value) {}
	};

	class DeviceQueueUsage : public Enum<ui32>
	{
	public:
		constexpr static ui32 Present = BIT(0);
		constexpr static ui32 Graphics = BIT(1);
		constexpr static ui32 Compute = BIT(2);
		constexpr static ui32 RayTrace = BIT(4);

		//constexpr static ui32 Transfer = BIT(0); //TODO: shall we support this?

		DeviceQueueUsage(ui32 value) : Enum<ui32>(value) {}
	};

	struct DeviceQueueFamily
	{
		siz index = SIZ_MAX;
		siz count = 0;
		DeviceQueueUsage usage = DeviceQueueUsage::None;
	};

	struct Device : public DetailObject
	{
		static mem::sptr<Device> Create(mem::sptr<DetailInstance> instance, DeviceUsage usage, mem::sptr<PresentTarget> target = nullptr);

		virtual ~Device() = default;

		virtual mem::sptr<DetailInstance> GetDetailInstance() const = 0;

		virtual mem::sptr<PresentTarget> GetPresentTarget() const = 0;

		virtual con::vector<DeviceQueueFamily> GetDeviceQueueFamilies() const = 0;

		virtual mem::sptr<PipelineCache> GetPipelineCache() const = 0;

		virtual mem::sptr<Fence> CreateFence() = 0;

		virtual mem::sptr<Semaphore> CreateSemaphore() = 0;

		//virtual mem::sptr<PipelineCache> CreatePipelineCache(str filename) const = 0; //TODO: how do we want to do this?
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_DEVICE_HPP */