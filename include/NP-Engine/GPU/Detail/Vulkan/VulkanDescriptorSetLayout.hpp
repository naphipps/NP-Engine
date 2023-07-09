//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/12/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP
#define NP_ENGINE_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanLogicalDevice.hpp"

namespace np::gpu::__detail
{
	class VulkanDescriptorSetLayout
	{
	private:
		mem::sptr<VulkanLogicalDevice> _device;
		const con::vector<VkDescriptorSetLayoutBinding> _layout_bindings;
		VkDescriptorSetLayout _layout;

		static VkDescriptorSetLayout CreateLayout(mem::sptr<VulkanLogicalDevice> device,
												  const con::vector<VkDescriptorSetLayoutBinding>& layout_bindings)
		{
			VkDescriptorSetLayout layout = nullptr;
			VkDescriptorSetLayoutCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			info.bindingCount = (ui32)layout_bindings.size();
			info.pBindings = layout_bindings.data();

			if (vkCreateDescriptorSetLayout(*device, &info, nullptr, &layout) != VK_SUCCESS)
				layout = nullptr;

			return layout;
		}

	public:
		VulkanDescriptorSetLayout(mem::sptr<VulkanLogicalDevice> device):
			_device(device),
			_layout_bindings(
				{/*
				 VkDescriptorSetLayoutBinding
				 {
					 uint32_t binding;
					 VkDescriptorType descriptorType;
					 uint32_t descriptorCount;
					 VkShaderStageFlags stageFlags;
					 const VkSampler* pImmutableSamplers;
				 }
				 */
				 {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, nullptr},
				 {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr}}),
			_layout(CreateLayout(_device, _layout_bindings))
		{}

		~VulkanDescriptorSetLayout()
		{
			if (_layout)
			{
				vkDestroyDescriptorSetLayout(*_device, _layout, nullptr);
				_layout = nullptr;
			}
		}

		operator VkDescriptorSetLayout() const
		{
			return _layout;
		}

		mem::sptr<VulkanLogicalDevice> GetLogicalDevice() const
		{
			return _device;
		}

		::std::optional<ui32> GetBindingForDescriptorType(VkDescriptorType descriptor_type) const
		{
			::std::optional<ui32> binding;
			for (const VkDescriptorSetLayoutBinding& layout_binding : _layout_bindings)
				if (layout_binding.descriptorType == descriptor_type)
				{
					binding = layout_binding.binding;
					break;
				}

			return binding;
		}

		const con::vector<VkDescriptorSetLayoutBinding>& GetBindings() const
		{
			return _layout_bindings;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP */