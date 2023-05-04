//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/12/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_DEVICE_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_DEVICE_HPP

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_DEVICE_RESOURCE_POOL_DEFAULT_SIZE
	#define NP_ENGINE_GRAPHICS_INTERFACE_DEVICE_POOL_RESOURCE_DEFAULT_SIZE 500
#endif

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Uid/Uid.hpp"

#include "RenderableObject.hpp"
#include "Resource.hpp"
#include "DeviceResourcePool.hpp"

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
//	^ this means that device.unregister(id) can remove device's sptr, so whenever the frame packet it done with it, then it will destroy itself then
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
minimap_scene.register(visible, minimap_id) // this should succeed whether or not minimap_id is associated with actual rendering data
minimap_scene.render() //every id will submitted to a job for visible-calculation-then-command-list-submission, but before that, the job will check if "device.HasResource(id)"
	^ since device.HasResource(minimap_id) would be false, then that job would resolve there
	^ this is more strain on the scene and job system, but enables us to reserve a spot before we have rendering data on the device

*/

/*
	TODO: add RenderableShape -- lines, circulars, polygons
		- support different ways to draw these (like GL_LINE, GL_LINES, etc)
		- <http://www.dgp.toronto.edu/~ah/csc418/fall_2001/tut/ogl_draw.html>
		- <https://www.khronos.org/opengl/wiki/Primitive> their vertex convetions are nice
*/

namespace np::gpu
{
	enum ShapeType : ui32
	{
		None = 0,
		Points,
		Line,
		Lines,
		Polygon,
		Quads, //TODO: I feel like this could be two diffent kinds since the interal triangles could be one of two different kinds
		QuadStrip, //TODO: ^ same here
		Triangles,
		TriangleStrip,
		TriangleFan,
		Circle, //TODO: two vertices per circle, center point, then radius point, create a circle in whatever way performs best

		Loop = Polygon,
		LineStrip = Line,
		LineLoop = Polygon
	};
}

/*
	TODO: encapsulate all vulkan commands, and bring some standard naming to the interface
		- we actually might be able to get away without having to bring these commands to the interface
		- although I do like the idea of giving more control to the interface...
*/

namespace np::gpu
{
	class Device
	{
	protected:
		mutexed_wrapper<con::umap<uid::Uid, mem::sptr<Resource>>> _resources; //TODO: is umap the best here? what about vector?

	public:
		virtual void Register(uid::Uid id, mem::sptr<Resource> resource)
		{
			(*_resources.get_access())[id] = resource;
		}

		virtual void Unregister(uid::Uid id)
		{
			_resources.get_access()->erase(id);
		}

		virtual void CleanupResources()
		{
			uid::UidSystem& uid_system = GetServices()->GetUidSystem();
			auto resources = _resources.get_access();
			for (auto it = resources->begin(); it != resources->end();)
			{
				if (!uid_system.Has(it->first))
					it = resources->erase(it);
				else
					it++;
			}
		}

		virtual mem::sptr<Resource> GetResource(uid::Uid id)
		{
			auto resources = _resources.get_access();
			auto it = resources->find(id);
			return it != resources->end() ? it->second : nullptr;
		}

		virtual bl HasResource(uid::Uid id)
		{
			auto resources = _resources.get_access();
			return resources->find(id) != resources->end();
		}

		virtual mem::sptr<DetailInstance> GetInstance() const = 0;

		virtual mem::sptr<srvc::Services> GetServices() const = 0;
	};
}

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_DEVICE_HPP */