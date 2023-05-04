//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/17/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_DESCRIPTOR_SETS_HPP
#define NP_ENGINE_VULKAN_DESCRIPTOR_SETS_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanDescriptorSetLayout.hpp"
#include "VulkanRenderContext.hpp"

namespace np::gpu::__detail
{
	class VulkanDescriptorSets
	{
	private:
		mem::sptr<VulkanLogicalDevice> _device;
		mem::sptr<VulkanDescriptorSetLayout> _descriptor_set_layout;

		VkDescriptorPool _descriptor_pool;
		con::vector<VkDescriptorSet> _descriptor_sets;

		static VkDescriptorPool CreateDescriptorPool(mem::sptr<VulkanLogicalDevice> device, ui32 sets_count, mem::sptr<VulkanDescriptorSetLayout> layout)
		{
			ui32 size = 10; //TODO: where does this value come from??
			con::vector<VkDescriptorPoolSize> pool_sizes;
			for (const VkDescriptorSetLayoutBinding& binding : layout->GetBindings())
				pool_sizes.emplace_back(VkDescriptorPoolSize{binding.descriptorType, size});

			VkDescriptorPoolCreateInfo create_info{};
			create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			create_info.poolSizeCount = (ui32)pool_sizes.size();
			create_info.pPoolSizes = pool_sizes.data();
			create_info.maxSets = sets_count;
			create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

			VkDescriptorPool pool = nullptr;
			if (vkCreateDescriptorPool(*device, &create_info, nullptr, &pool) != VK_SUCCESS)
				pool = nullptr;
			return pool;
		}

		static con::vector<VkDescriptorSet> CreateDescriptorSets(mem::sptr<VulkanLogicalDevice> device, ui32 sets_count, mem::sptr<VulkanDescriptorSetLayout> layout, VkDescriptorPool pool)
		{
			con::vector<VkDescriptorSetLayout> layouts(sets_count, *layout);

			VkDescriptorSetAllocateInfo allocate_info{};
			allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocate_info.descriptorPool = pool;
			allocate_info.descriptorSetCount = (ui32)layouts.size();
			allocate_info.pSetLayouts = layouts.data();

			con::vector<VkDescriptorSet> sets(layouts.size());
			if (vkAllocateDescriptorSets(*device, &allocate_info, sets.data()) != VK_SUCCESS)
				sets.clear();

			return sets;
		}

	public:
		VulkanDescriptorSets(mem::sptr<VulkanLogicalDevice> device, ui32 sets_count, mem::sptr<VulkanDescriptorSetLayout> descriptor_set_layout) :
			_device(device),
			_descriptor_set_layout(descriptor_set_layout),
			_descriptor_pool(CreateDescriptorPool(device, sets_count, _descriptor_set_layout)),
			_descriptor_sets(CreateDescriptorSets(device, sets_count, _descriptor_set_layout, _descriptor_pool))
		{}

		~VulkanDescriptorSets()
		{
			if (_descriptor_pool)
			{
				vkDestroyDescriptorPool(*_device, _descriptor_pool, nullptr);
				_descriptor_pool = nullptr;
			}
		}

		operator const con::vector<VkDescriptorSet>&() const
		{
			return _descriptor_sets;
		}

		VkDescriptorSet operator[](i32 index) const
		{
			return _descriptor_sets[index];
		}

		mem::sptr<VulkanLogicalDevice> GetLogicalContext() const
		{
			return _device;
		}

		mem::sptr<VulkanDescriptorSetLayout> GetLayout() const
		{
			return _descriptor_set_layout;
		}

		void Rebuild(ui32 sets_count)
		{
			vkDestroyDescriptorPool(*_device, _descriptor_pool, nullptr);
			_descriptor_pool = CreateDescriptorPool(_device, sets_count, _descriptor_set_layout);
			_descriptor_sets = CreateDescriptorSets(_device, sets_count, _descriptor_set_layout, _descriptor_pool);
		}

		void SubmitWriter(VkWriteDescriptorSet& writer, siz dst_set_index)
		{
			writer.dstBinding = _descriptor_set_layout->GetBindingForDescriptorType(writer.descriptorType).value();
			writer.dstSet = _descriptor_sets[dst_set_index];
			vkUpdateDescriptorSets(*_device, 1, mem::AddressOf(writer), 0, nullptr);
		}

		void SubmitWriters(con::vector<VkWriteDescriptorSet>& writers, siz dst_set_index)
		{
			for (VkWriteDescriptorSet& writer : writers)
			{
				writer.dstBinding = _descriptor_set_layout->GetBindingForDescriptorType(writer.descriptorType).value();
				writer.dstSet = _descriptor_sets[dst_set_index];
			}

			vkUpdateDescriptorSets(*_device, (ui32)writers.size(), writers.data(), 0, nullptr);
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_DESCRIPTOR_SETS_HPP */