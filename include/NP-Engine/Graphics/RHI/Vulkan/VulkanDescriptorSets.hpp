//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/17/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_DESCRIPTOR_SETS_HPP
#define NP_ENGINE_VULKAN_DESCRIPTOR_SETS_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanSwapchain.hpp"
#include "VulkanDescriptorSetLayout.hpp"

namespace np::graphics::rhi
{
	class VulkanDescriptorSets
	{
	private:
		VulkanSwapchain& _swapchain;
		VulkanDescriptorSetLayout& _descriptor_set_layout;

		VkDescriptorPool _descriptor_pool;
		container::vector<VkDescriptorSet> _descriptor_sets;

		VkDescriptorPool CreateDescriptorPool()
		{
			VkDescriptorPool pool = nullptr;
			ui32 size = 10;
			container::vector<VkDescriptorPoolSize> pool_sizes;

			for (const VkDescriptorSetLayoutBinding& binding : _descriptor_set_layout.GetBindings())
				pool_sizes.emplace_back(VkDescriptorPoolSize{binding.descriptorType, size});

			VkDescriptorPoolCreateInfo create_info{};
			create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			create_info.poolSizeCount = (ui32)pool_sizes.size();
			create_info.pPoolSizes = pool_sizes.data();
			create_info.maxSets = NP_ENGINE_VULKAN_MAX_FRAME_COUNT;
			create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

			if (vkCreateDescriptorPool(GetDevice(), &create_info, nullptr, &pool) != VK_SUCCESS)
				pool = nullptr;
			return pool;
		}

		container::vector<VkDescriptorSet> CreateDescriptorSets()
		{
			container::vector<VkDescriptorSet> sets;
			container::vector<VkDescriptorSetLayout> layouts(NP_ENGINE_VULKAN_MAX_FRAME_COUNT, _descriptor_set_layout);

			VkDescriptorSetAllocateInfo allocate_info{};
			allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocate_info.descriptorPool = _descriptor_pool;
			allocate_info.descriptorSetCount = (ui32)layouts.size();
			allocate_info.pSetLayouts = layouts.data();

			sets.resize(layouts.size());
			if (vkAllocateDescriptorSets(GetDevice(), &allocate_info, sets.data()) != VK_SUCCESS)
				sets.clear();

			return sets;
		}

	public:
		VulkanDescriptorSets(VulkanSwapchain& swapchain, VulkanDescriptorSetLayout& descriptor_set_layout):
			_swapchain(swapchain),
			_descriptor_set_layout(descriptor_set_layout),
			_descriptor_pool(CreateDescriptorPool()),
			_descriptor_sets(CreateDescriptorSets())
		{}

		~VulkanDescriptorSets()
		{
			if (_descriptor_pool)
				vkDestroyDescriptorPool(GetDevice(), _descriptor_pool, nullptr);
		}

		operator const container::vector<VkDescriptorSet>&() const
		{
			return _descriptor_sets;
		}

		VkDescriptorSet operator[](i32 index) const
		{
			return _descriptor_sets[index];
		}

		VulkanSwapchain& GetSwapchain()
		{
			return _swapchain;
		}

		const VulkanSwapchain& GetSwapchain() const
		{
			return _swapchain;
		}

		VulkanDevice& GetDevice()
		{
			return _swapchain.GetDevice();
		}

		const VulkanDevice& GetDevice() const
		{
			return _swapchain.GetDevice();
		}

		VkDescriptorSetLayout GetLayout() const
		{
			return _descriptor_set_layout;
		}

		void Rebuild()
		{
			vkDestroyDescriptorPool(GetDevice(), _descriptor_pool, nullptr);
			_descriptor_pool = CreateDescriptorPool();
			_descriptor_sets = CreateDescriptorSets();
		}

		void SubmitWriter(VkWriteDescriptorSet& writer)
		{
			_descriptor_set_layout.GetBindingForDescriptorType(writer.descriptorType, writer.dstBinding);
			writer.dstSet = _descriptor_sets[GetSwapchain().GetCurrentImageIndex()];
			vkUpdateDescriptorSets(GetDevice(), 1, memory::AddressOf(writer), 0, nullptr);
		}

		void SubmitWriters(container::vector<VkWriteDescriptorSet>& writers)
		{
			for (VkWriteDescriptorSet& writer : writers)
			{
				_descriptor_set_layout.GetBindingForDescriptorType(writer.descriptorType, writer.dstBinding);
				writer.dstSet = _descriptor_sets[GetSwapchain().GetCurrentImageIndex()];
			}

			vkUpdateDescriptorSets(GetDevice(), (ui32)writers.size(), writers.data(), 0, nullptr);
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_DESCRIPTOR_SETS_HPP */