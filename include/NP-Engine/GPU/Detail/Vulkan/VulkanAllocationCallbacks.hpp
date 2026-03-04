//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/20/25
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_ALLOCATION_CALLBACKS_HPP
#define NP_ENGINE_GPU_VULKAN_ALLOCATION_CALLBACKS_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

namespace np::gpu::__detail
{
	class VulkanAllocationCallbacks
	{
	private:
		mem::sptr<srvc::Services> _services;
		VkAllocationCallbacks _callbacks;

		static void* Allocate(void* user_data, siz size, siz alignment, VkSystemAllocationScope scope)
		{
			VulkanAllocationCallbacks* callbacks = static_cast<VulkanAllocationCallbacks*>(user_data);
			mem::sptr<srvc::Services> services = callbacks->GetServices();
			mem::Allocator& allocator = services->GetAllocator();
			mem::Block block = allocator.Allocate(size); //TODO: consider alignment
			return block.ptr;
		}

		static void* Reallocate(void* user_data, void* old_ptr, siz size, siz alignment, VkSystemAllocationScope scope)
		{
			VulkanAllocationCallbacks* callbacks = static_cast<VulkanAllocationCallbacks*>(user_data);
			mem::sptr<srvc::Services> services = callbacks->GetServices();
			mem::Allocator& allocator = services->GetAllocator();
			mem::Block block = allocator.Reallocate(old_ptr, size); //TODO: consider alignment
			return block.ptr;
		}

		static void Deallocate(void* user_data, void* ptr)
		{
			VulkanAllocationCallbacks* callbacks = static_cast<VulkanAllocationCallbacks*>(user_data);
			mem::sptr<srvc::Services> services = callbacks->GetServices();
			mem::Allocator& allocator = services->GetAllocator();
			allocator.Deallocate(ptr);
		}

		static void AllocateNotification(void* user_data, siz size, VkInternalAllocationType type,
										 VkSystemAllocationScope scope)
		{
			//TODO: add some logging?
		}

		static void DeallocateNotification(void* user_data, siz size, VkInternalAllocationType type,
										   VkSystemAllocationScope scope)
		{
			//TODO: add some logging?
		}

		VkAllocationCallbacks CreateVkAllocationCallbacks()
		{
			return {this, Allocate, Reallocate, Deallocate, AllocateNotification, DeallocateNotification};
		}

	public:
		VulkanAllocationCallbacks(mem::sptr<srvc::Services> services):
			_services(services),
			_callbacks(CreateVkAllocationCallbacks())
		{}

		//TODO: rule of 5 here? pretty sure, might as well

		operator const VkAllocationCallbacks*() const
		{
			return mem::AddressOf(_callbacks);
		}

		virtual mem::sptr<srvc::Services> GetServices() const
		{
			return _services;
		}
	};
} //namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_ALLOCATION_CALLBACKS_HPP */