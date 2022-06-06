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

#include "VulkanDevice.hpp"

namespace np::graphics::rhi
{
	class VulkanDescriptorSetLayout
	{
	private:
		VulkanDevice& _device;
		const container::vector<VkDescriptorSetLayoutBinding> _layout_bindings;
		VkDescriptorSetLayout _layout;

		VkDescriptorSetLayout CreateLayout() const
		{
			VkDescriptorSetLayout layout = nullptr;
			VkDescriptorSetLayoutCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			info.bindingCount = (ui32)_layout_bindings.size();
			info.pBindings = _layout_bindings.data();

			if (vkCreateDescriptorSetLayout(GetDevice(), &info, nullptr, &layout) != VK_SUCCESS)
				layout = nullptr;

			return layout;
		}

	public:
		VulkanDescriptorSetLayout(VulkanDevice& device):
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
			_layout(CreateLayout())
		{}

		~VulkanDescriptorSetLayout()
		{
			if (_layout)
			{
				vkDestroyDescriptorSetLayout(GetDevice(), _layout, nullptr);
				_layout = nullptr;
			}
		}

		operator VkDescriptorSetLayout() const
		{
			return _layout;
		}

		VulkanDevice& GetDevice()
		{
			return _device;
		}

		const VulkanDevice& GetDevice() const
		{
			return _device;
		}

		bl GetBindingForDescriptorType(VkDescriptorType descriptor_type, ui32& out_binding) const
		{
			bl found = false;
			for (const VkDescriptorSetLayoutBinding& layout_binding : _layout_bindings)
			{
				if (layout_binding.descriptorType == descriptor_type)
				{
					found = true;
					out_binding = layout_binding.binding;
					break;
				}
			}
			return found;
		}

		const container::vector<VkDescriptorSetLayoutBinding>& GetBindings() const
		{
			return _layout_bindings;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP */