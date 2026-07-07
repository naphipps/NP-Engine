//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/3/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_RESOURCE_HPP
#define NP_ENGINE_GPU_VULKAN_RESOURCE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Resource.hpp"
#include "NP-Engine/GPU/Interface/BufferResource.hpp"
#include "NP-Engine/GPU/Interface/ImageResource.hpp"

#include "VulkanDevice.hpp"
#include "VulkanStage.hpp"

namespace np::gpu::__detail
{
	class VulkanResourceType : public ResourceType //TODO: I think we can remove VulkanResourceType
	{
	public:
		VulkanResourceType(ui32 value): ResourceType(value) {}

		//TODO: get Vk things
	};

	class VulkanResourceUsage : public ResourceUsage  //TODO: I think we can remove VulkanResourceUsage
	{
	public:
		VulkanResourceUsage(ui32 value): ResourceUsage(value) {}

		//TODO: get Vk things
	};

	class VulkanResourceOperation : public ResourceOperation
	{
	public:
		VulkanResourceOperation(ui32 value): ResourceOperation(value) {}

		VkAttachmentLoadOp GetVkAttachmentLoadOp() const
		{
			VkAttachmentLoadOp op = VK_ATTACHMENT_LOAD_OP_MAX_ENUM;

			if (Equals(Load))
				op = VK_ATTACHMENT_LOAD_OP_LOAD;
			else if (Equals(Load | Clear))
				op = VK_ATTACHMENT_LOAD_OP_CLEAR;
			else if (Equals(Load | DontCare))
				op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

			return op;
		}

		VkAttachmentStoreOp GetVkAttachmentStoreOp() const
		{
			VkAttachmentStoreOp op = VK_ATTACHMENT_STORE_OP_MAX_ENUM;

			if (Equals(Store))
				op = VK_ATTACHMENT_STORE_OP_STORE;
			else if (Equals(Store | DontCare))
				op = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			return op;
		}
	};

	struct VulkanResourceDescription
	{
		VulkanResourceType type = VulkanResourceType::None; //TODO: we might be just fine with ResourceType
		VulkanResourceUsage usage = VulkanResourceUsage::None; //TODO: we might be just fine with ResourceUsage
		ui32 count = 0;
		VulkanStage stage = VulkanStage::None;

		VulkanResourceDescription(const ResourceDescription& other = {}):
			type(other.type),
			usage(other.usage),
			count(other.count),
			stage(other.stage)
		{}

		operator ResourceDescription() const
		{
			return {type, usage, count, stage};
		}

		bl operator==(const VulkanResourceDescription& other) const
		{
			return type == other.type && usage == other.usage && count == other.count && stage == other.stage;
		}

		VkDescriptorType GetVkDescriptorType() const
		{
			//		VkDescriptorType type{};

			//		if (_value == Sampler) //TODO: what comparison should we use? Contains? Any? straight equals?
			//			type = VK_DESCRIPTOR_TYPE_SAMPLER;
			//		/*if (_value == (Sampler | Image))
			//			type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;*/

			//		/*
			//			TODO: figure out VkDescriptorType operator here -- this is an actual enum and NOT flags
			//				- [x] VK_DESCRIPTOR_TYPE_SAMPLER = 0,
			//				- [ ] VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
			//				- [ ] VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
			//				- [ ] VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
			//				- [x] VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
			//				- [x] VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
			//				- [x] VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
			//				- [x] VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
			//				- [x] VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
			//				- [x] VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
			//				- [ ] VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,
			//				- [ ] VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK = 1000138000,
			//		*/

			//		return type;
			VkDescriptorType type = VK_DESCRIPTOR_TYPE_MAX_ENUM;

			switch (this->type)
			{
			case ResourceType::Buffer:
			{
				BufferResourceUsage usage{ this->usage };
				if (usage.Equals(BufferResourceUsage::Uniform | BufferResourceUsage::Texel))
					type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
				else if (usage.Equals(BufferResourceUsage::Storage | BufferResourceUsage::Texel))
					type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
				else if (usage.Equals(BufferResourceUsage::Uniform | BufferResourceUsage::Dynamic))
					type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
				else if (usage.Equals(BufferResourceUsage::Storage | BufferResourceUsage::Dynamic))
					type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
				else if (usage.Equals(BufferResourceUsage::Uniform))
					type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				else if (usage.Equals(BufferResourceUsage::Storage))
					type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			}
			break;
			case ResourceType::Image:
			{
				ImageResourceUsage usage{ this->usage };
			}
			break;
			case ResourceType::Sampler:
			{
				type = VK_DESCRIPTOR_TYPE_SAMPLER;
			}
			break;
			}

			return type;
		}

		VkDescriptorSetLayoutBinding GetVkDescriptorSetLayoutBinding() const
		{
			const VulkanStage stage{ this->stage };
			VkDescriptorSetLayoutBinding binding{};
			binding.binding = UI32_MAX; //intentional invalid value
			binding.stageFlags = stage.GetVkShaderStageFlags();
			binding.descriptorType = GetVkDescriptorType();
			binding.descriptorCount = count;
			return binding;
		}
	};

	class VulkanResourceLayout : public ResourceLayout
	{
	protected:
		mem::sptr<VulkanDevice> _device;
		con::vector<VulkanResourceDescription> _descriptions;
		VkDescriptorSetLayout _layout;

		static VkDescriptorSetLayoutCreateInfo CreateInfo()
		{
			VkDescriptorSetLayoutCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			return info;
		}

		static VkDescriptorSetLayout CreateVkDescriptorSetLayout(mem::sptr<VulkanDevice> device,
																 const con::vector<VulkanResourceDescription>& descriptions)
		{
			con::vector<VkDescriptorSetLayoutBinding> bindings(descriptions.size());
			for (siz i = 0; i < bindings.size(); i++)
			{
				bindings[i] = descriptions[i].GetVkDescriptorSetLayoutBinding();
				bindings[i].binding = i;
			}

			VkDescriptorSetLayoutCreateInfo info = CreateInfo();
			info.bindingCount = (ui32)bindings.size();
			info.pBindings = bindings.empty() ? nullptr : bindings.data();

			mem::sptr<VulkanInstance> instance = device->GetDetailInstance();
			VkDescriptorSetLayout layout = nullptr;
			VkResult result = vkCreateDescriptorSetLayout(*device->GetLogicalDevice(), &info, instance->GetVulkanAllocationCallbacks(), &layout);
			return result == VK_SUCCESS ? layout : nullptr;
		}

	public:
		VulkanResourceLayout(mem::sptr<Device> device, con::vector<ResourceDescription> descriptions):
			//TODO: ^ make sure all the things are checking our physical device's limits
			_device(device),
			_descriptions(descriptions.begin(), descriptions.end()),
			_layout(CreateVkDescriptorSetLayout(_device, _descriptions))
		{}

		virtual ~VulkanResourceLayout()
		{
			if (_layout)
			{
				mem::sptr<VulkanInstance> instance = _device->GetDetailInstance();
				vkDestroyDescriptorSetLayout(*_device->GetLogicalDevice(), _layout, instance->GetVulkanAllocationCallbacks());
				_layout = nullptr;
			}
		}

		operator VkDescriptorSetLayout() const
		{
			return _layout;
		}

		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan; //TODO: use enum everywhere
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _device->GetServices();
		}

		virtual mem::sptr<Device> GetDevice() const override
		{
			return _device;
		}

		virtual con::vector<ResourceDescription> GetResourceDescriptions() const override
		{
			return {_descriptions.begin(), _descriptions.end()};
		}

		virtual bl IsCompatible(mem::sptr<ResourceLayout> other_) const override
		{
			mem::sptr<VulkanResourceLayout> other = DetailObject::EnsureIsDetailType(other_, DetailType::Vulkan);
			bl is = other && _descriptions.size() == other->_descriptions.size();

			for (siz i = 0; is && i < _descriptions.size(); i++)
				is &= _descriptions[i] == other->_descriptions[i];

			return is;
		}
	};
} //namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_RESOURCE_HPP */