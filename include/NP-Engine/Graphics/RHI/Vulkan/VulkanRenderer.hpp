//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_VULKAN_RENDERER_HPP
#define NP_ENGINE_GRAPHICS_VULKAN_RENDERER_HPP

#include <iostream> //TODO: remove

//#include <vulkan/vulkan.hpp>
//#include <GLFW/glfw3.h>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Window/Window.hpp"

#include "../../RPI/Renderer.hpp"
#include "../../RPI/RhiType.hpp"

#include "VulkanDevice.hpp"

//TODO: add summary comments

namespace np::graphics::rhi
{
	class VulkanRenderer : public Renderer
	{
	private:

		memory::Allocator& _allocator;
		VulkanDevice* _device;

	public:

		VulkanRenderer():
		_allocator(memory::DefaultTraitAllocator),
		_device(nullptr)
		{}

		~VulkanRenderer()
		{
			if (_device != nullptr)
			{
				memory::Destruct(_device);
				_allocator.Deallocate(_device);
			}
		}

		RhiType GetRhiType() const override
		{
			return RhiType::Vulkan;
		}

		str GetName() const override
		{
			return "Vulkan";
		}

		void AttachToWindow(window::Window& window) override
		{
			memory::Block block = _allocator.Allocate(sizeof(VulkanDevice));
			memory::Construct<VulkanDevice>(block, window);
			_device = (VulkanDevice*)block.ptr;

			std::cout << "Renderer Attaching to Window\n";
			//set up our surface
			//create our device
			//this is all tied together, so we might move all this to device...
		}
	};
}

#endif /* NP_ENGINE_GRAPHICS_VULKAN_RENDERER_HPP */