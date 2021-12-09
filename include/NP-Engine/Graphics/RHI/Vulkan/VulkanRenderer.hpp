//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_RENDERER_HPP
#define NP_ENGINE_VULKAN_RENDERER_HPP

#include <iostream> //TODO: remove
#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Window/Window.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "../../RPI/Renderer.hpp"
#include "../../RPI/RhiType.hpp"

#include "VulkanInstance.hpp"
#include "VulkanSurface.hpp"
#include "VulkanDevice.hpp"
#include "VulkanShader.hpp"
#include "VulkanPipeline.hpp"

// TODO: add summary comments

namespace np::graphics::rhi
{
	class VulkanRenderer : public Renderer
	{
	private:
		memory::Allocator& _allocator;
		container::vector<VulkanInstance*> _instances;
		container::vector<VulkanSurface*> _surfaces;
		container::vector<VulkanDevice*> _devices; // TODO: should these be sptr??
		container::vector<VulkanPipeline*> _pipelines;

		template <class T, class... Args>
		T* Create(container::vector<T*>& v, Args&&... args)
		{
			memory::Block block = _allocator.Allocate(sizeof(T));
			memory::Construct<T>(block, ::std::forward<Args>(args)...);
			return v.emplace_back((T*)block.ptr);
		}

		template <class T>
		void DestroyVector(container::vector<T*>& ptrs)
		{
			for (T* ptr : ptrs)
			{
				memory::Destruct(ptr);
				_allocator.Deallocate(ptr);
			}
		}

	public:
		VulkanRenderer(): _allocator(memory::DefaultTraitAllocator)
		{
			Create<VulkanInstance>(_instances);
		}

		~VulkanRenderer()
		{
			DestroyVector<VulkanPipeline>(_pipelines);
			DestroyVector<VulkanDevice>(_devices);
			DestroyVector<VulkanSurface>(_surfaces);
			DestroyVector<VulkanInstance>(_instances);
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
			VulkanSurface& surface = *Create<VulkanSurface>(_surfaces, *_instances.front(), window);
			VulkanDevice& device = *Create<VulkanDevice>(_devices, surface);
			VulkanPipeline& pipeline = *Create<VulkanPipeline>(_pipelines, device);
		}

		void DetachFromWindow(window::Window& window) override
		{
			std::cout << "implement VulkanRenderer::DetachFromWindow\n";
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_RENDERER_HPP */