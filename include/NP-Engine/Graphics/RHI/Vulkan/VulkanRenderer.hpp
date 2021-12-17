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
#include "NP-Engine/Time/Time.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "../../RPI/Renderer.hpp"
#include "../../RPI/RhiType.hpp"

#include "VulkanInstance.hpp"
#include "VulkanSurface.hpp"
#include "VulkanDevice.hpp"
#include "VulkanShader.hpp"
#include "VulkanSwapchain.hpp"
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
		void DestroyInVector(container::vector<T*>& ptrs, const T* t)
		{
			for (auto it = ptrs.begin(); it != ptrs.end(); it++)
			{
				if (t == *it)
				{
					memory::Destruct(*it);
					_allocator.Deallocate(*it);
					ptrs.erase(it);
					break;
				}
			}
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
			for (VulkanDevice* device : _devices)
				vkDeviceWaitIdle(*device);

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
			for (auto it = _pipelines.begin(); it != _pipelines.end(); it++)
			{
				if (memory::AddressOf((*it)->Surface().Window()) == memory::AddressOf(window))
				{
					VulkanPipeline* pipeline = *it;
					VulkanDevice* device = memory::AddressOf(pipeline->Device());
					VulkanSurface* surface = memory::AddressOf(pipeline->Surface());
					VulkanInstance* instance = memory::AddressOf(pipeline->Instance());

					memory::Destruct(pipeline);
					_allocator.Deallocate(pipeline);
					_pipelines.erase(it);

					DestroyInVector(_devices, device);
					DestroyInVector(_surfaces, surface);
					DestroyInVector(_instances, instance);
					break;
				}
			}
		}

		void Draw(time::DurationMilliseconds time_delta) override
		{
			for (VulkanPipeline* pipeline : _pipelines)
				pipeline->Draw(time_delta);
		}

		void AdjustForWindowResize(window::Window& window) override {}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_RENDERER_HPP */