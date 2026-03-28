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
			mem::allocator& a = services->GetAllocator();
			mem::block b = a.allocate(size, alignment);
			return b.ptr;
		}

		static void* Reallocate(void* user_data, void* ptr, siz size, siz alignment, VkSystemAllocationScope scope)
		{
			VulkanAllocationCallbacks* callbacks = static_cast<VulkanAllocationCallbacks*>(user_data);
			mem::sptr<srvc::Services> services = callbacks->GetServices();
			mem::allocator& a = services->GetAllocator();
			mem::block b = a.reallocate(ptr, size, alignment);
			return b.ptr;
		}

		static void Deallocate(void* user_data, void* ptr)
		{
			VulkanAllocationCallbacks* callbacks = static_cast<VulkanAllocationCallbacks*>(user_data);
			mem::sptr<srvc::Services> services = callbacks->GetServices();
			mem::allocator& a = services->GetAllocator();
			a.deallocate(ptr);
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
			return mem::address_of(_callbacks);
		}

		virtual mem::sptr<srvc::Services> GetServices() const
		{
			return _services;
		}
	};
} //namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_ALLOCATION_CALLBACKS_HPP */