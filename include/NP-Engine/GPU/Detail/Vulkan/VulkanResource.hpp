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

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanDevice.hpp"
#include "VulkanStage.hpp"

namespace np::gpu::__detail
{
	class VulkanResourceType : public ResourceType
	{
	public:
		VulkanResourceType(ui32 value): ResourceType(value) {}

		//TODO: get Vk things
	};

	class VulkanResourceUsage : public ResourceUsage
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

			if (ContainsAll(Load))
				op = VK_ATTACHMENT_LOAD_OP_LOAD;
			else if (ContainsAll(Load | Clear))
				op = VK_ATTACHMENT_LOAD_OP_CLEAR;
			else if (ContainsAll(Load | DontCare))
				op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

			return op;
		}

		VkAttachmentStoreOp GetVkAttachmentStoreOp() const
		{
			VkAttachmentStoreOp op = VK_ATTACHMENT_STORE_OP_MAX_ENUM;

			if (ContainsAll(Store))
				op = VK_ATTACHMENT_STORE_OP_STORE;
			else if (ContainsAll(Store | DontCare))
				op = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			return op;
		}
	};

	struct VulkanResourceDescription //TODO: remove inheritance here to use Vulkan objects as fields
	{
		VulkanResourceType type = VulkanResourceType::None;
		VulkanResourceUsage usage = VulkanResourceUsage::None;
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
				BufferResourceUsage usage = (ui32)this->usage;
				if (usage.ContainsAll(BufferResourceUsage::Uniform | BufferResourceUsage::Texel))
					type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
				else if (usage.ContainsAll(BufferResourceUsage::Storage | BufferResourceUsage::Texel))
					type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
				else if (usage.ContainsAll(BufferResourceUsage::Uniform | BufferResourceUsage::Dynamic))
					type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
				else if (usage.ContainsAll(BufferResourceUsage::Storage | BufferResourceUsage::Dynamic))
					type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
				else if (usage.ContainsAll(BufferResourceUsage::Uniform))
					type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				else if (usage.ContainsAll(BufferResourceUsage::Storage))
					type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			}
			break;
			case ResourceType::Image:
			{
				ImageResourceUsage usage = (ui32)this->usage;
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
			const VulkanStage stage = this->stage;
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

			VkDescriptorSetLayout layout = nullptr;
			if (vkCreateDescriptorSetLayout(*device->GetLogicalDevice(), &info, nullptr, &layout) != VK_SUCCESS)
				layout = nullptr;
			return layout;
		}

	public:
		VulkanResourceLayout(mem::sptr<Device> device,
							 con::vector<ResourceDescription>
								 descriptions): //TODO: make sure all the things are checking our physical device's limits
			_device(device),
			_descriptions(descriptions.begin(), descriptions.end()),
			_layout(CreateVkDescriptorSetLayout(_device, _descriptions))
		{}

		virtual ~VulkanResourceLayout()
		{
			if (_layout)
			{
				vkDestroyDescriptorSetLayout(*_device->GetLogicalDevice(), _layout, nullptr);
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
	};

	class VulkanResourceGroup : public ResourceGroup
	{
	protected:
		mem::sptr<VulkanResourceLayout> _resource_layout;
		VkDescriptorSet _set;

	public:
		VulkanResourceGroup(mem::sptr<ResourceLayout> resource_layout, VkDescriptorSet set):
			_resource_layout(resource_layout),
			_set(set)
		{}

		virtual ~VulkanResourceGroup() = default;

		operator VkDescriptorSet() const
		{
			return _set;
		}

		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _resource_layout->GetServices();
		}

		virtual mem::sptr<ResourceLayout> GetResourceLayout() const override
		{
			return _resource_layout;
		}

		virtual siz GetResourceCount() const override
		{
			return _resource_layout->GetResourceDescriptions().size();
		}

		virtual bl AssignBufferResource(siz index, mem::sptr<BufferResource> resource, BufferResourceAssignInfo info) override
		{
			return false; //TODO: how do we want to handle this?
		}

		virtual bl AssignImageResource(siz index, mem::sptr<ImageResource> resource, ImageResourceAssignInfo info) override
		{
			return false; //TODO: how do we want to handle this?
		}
	};

	class VulkanResourceGroupPool : public ResourceGroupPool
	{
	protected:
		class VulkanResourceGroupDestroyer : public mem::smart_ptr_contiguous_destroyer<VulkanResourceGroup>
		{
		private:
			using base = mem::smart_ptr_contiguous_destroyer<VulkanResourceGroup>;
			mem::sptr<VulkanDevice> _device;
			VkDescriptorPool _pool;

		public:
			VulkanResourceGroupDestroyer(mem::sptr<VulkanDevice> device, VkDescriptorPool pool, mem::allocator& a):
				base(a),
				_device(device),
				_pool(pool)
			{}

			VulkanResourceGroupDestroyer(const VulkanResourceGroupDestroyer& other):
				base(other._allocator),
				_device(other._device),
				_pool(other._pool)
			{}

			VulkanResourceGroupDestroyer(VulkanResourceGroupDestroyer&& other) noexcept:
				base(other._allocator),
				_device(::std::move(other._device)),
				_pool(::std::move(other._pool))
			{
				other._device.reset();
				other._pool = nullptr;
			}

			void destruct_object(VulkanResourceGroup* ptr) override
			{
				VkDescriptorSet set = *ptr;
				vkFreeDescriptorSets(*_device->GetLogicalDevice(), _pool, 1,
									 &set); //TODO: do we do anything with this result? Other than an assert, I can't imagine
											//what we could do with it
				base::destruct_object(ptr);
			}
		};

		mem::sptr<VulkanDevice> _device;
		siz _size;
		con::vector<VulkanResourceDescription> _descriptions;
		VkDescriptorPool _pool;

		static VkDescriptorSetAllocateInfo GetVkDescriptorSetAllocateInfo(VkDescriptorPool pool, siz count)
		{
			VkDescriptorSetAllocateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			info.descriptorPool = pool;
			info.descriptorSetCount = count;
			return info;
		}

		static VkDescriptorSet CreateVkDescriptorSet(mem::sptr<VulkanResourceLayout> resource_layout,
													 VkDescriptorSetAllocateInfo info)
		{
			mem::sptr<VulkanDevice> device = resource_layout->GetDevice();
			VkDescriptorSetLayout layout = *resource_layout;

			info.descriptorSetCount = 1;
			info.pSetLayouts = &layout;

			VkDescriptorSet descriptor_set = nullptr;
			if (vkAllocateDescriptorSets(*device->GetLogicalDevice(), &info, &descriptor_set) != VK_SUCCESS)
				descriptor_set = nullptr;

			return descriptor_set;
		}

		static VkDescriptorPoolCreateInfo CreateVkInfo(siz size)
		{
			/*
				TODO: what are these create flags do?
					VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT = 0x00000001,
					VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT = 0x00000002,
					VK_DESCRIPTOR_POOL_CREATE_HOST_ONLY_BIT_EXT = 0x00000004,
					VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
					VK_DESCRIPTOR_POOL_CREATE_HOST_ONLY_BIT_VALVE = VK_DESCRIPTOR_POOL_CREATE_HOST_ONLY_BIT_EXT,
					VK_DESCRIPTOR_POOL_CREATE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
			*/

			VkDescriptorPoolCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			info.maxSets = size;
			return info;
		}

		static VkDescriptorPool CreateVkDescriptorPool(mem::sptr<VulkanDevice> device, siz size,
													   const con::vector<VulkanResourceDescription>& descriptions)
		{
			con::vector<VkDescriptorPoolSize> pool_sizes{};
			for (const VulkanResourceDescription& description : descriptions)
			{
				const VkDescriptorType type = description.GetVkDescriptorType();
				bl found = false;

				for (auto it = pool_sizes.rbegin(); !found && it != pool_sizes.rend(); it++)
				{
					found |= type == it->type;
					if (found)
						it->descriptorCount++;
				}

				if (!found)
					pool_sizes.emplace_back(VkDescriptorPoolSize{type, 1});
			}

			VkDescriptorPoolCreateInfo info = CreateVkInfo(size);
			info.poolSizeCount = pool_sizes.size();
			info.pPoolSizes = pool_sizes.empty() ? nullptr : pool_sizes.data();
			info.flags |= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; //enforce

			VkDescriptorPool pool = nullptr;
			if (vkCreateDescriptorPool(*device->GetLogicalDevice(), &info, nullptr, &pool) != VK_SUCCESS)
				pool = nullptr;
			return nullptr;
		}

	public:
		VulkanResourceGroupPool(mem::sptr<Device> device, siz size, con::vector<ResourceDescription> descriptions):
			_device(device),
			_size(size),
			_descriptions(descriptions.begin(), descriptions.end()),
			_pool(CreateVkDescriptorPool(_device, _size, _descriptions))
		{}

		virtual ~VulkanResourceGroupPool()
		{
			if (_pool)
			{
				vkDestroyDescriptorPool(*_device->GetLogicalDevice(), _pool, nullptr);
				_pool = nullptr;
			}
		}

		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _device->GetServices();
		}

		virtual mem::sptr<Device> GetDevice() const override
		{
			return _device;
		}

		virtual siz GetSize() const override
		{
			return _size;
		}

		virtual mem::sptr<ResourceGroup> CreateResourceGroup(mem::sptr<ResourceLayout> layout) override
		{
			using destroyer_type = VulkanResourceGroupDestroyer;
			using resource_type = mem::smart_ptr_resource<VulkanResourceGroup, destroyer_type>;
			using contiguous_block_type = mem::smart_ptr_contiguous_block<VulkanResourceGroup, resource_type>;

			mem::allocator& a = GetServices()->GetAllocator();
			resource_type* resource = nullptr;
			contiguous_block_type* contiguous_block = mem::create<contiguous_block_type>(a);

			if (contiguous_block)
			{
				mem::sptr<VulkanResourceLayout> vulkan_layout = DetailObject::EnsureIsDetailType(layout, DetailType::Vulkan);
				if (vulkan_layout)
				{
					const VkDescriptorSetLayout vk_layout = *vulkan_layout;

					VkDescriptorSetAllocateInfo info = GetVkDescriptorSetAllocateInfo(_pool, 1);
					info.pSetLayouts = &vk_layout;

					VkDescriptorSet set = nullptr;
					VkResult result = vkAllocateDescriptorSets(*_device->GetLogicalDevice(), &info, &set);

					if (result == VK_SUCCESS)
					{
						VulkanResourceGroup* object =
							mem::construct<VulkanResourceGroup>(contiguous_block->object_block, vulkan_layout, set);
						resource = mem::construct<resource_type>(contiguous_block->resource_block,
																 destroyer_type{_device, _pool, a}, object);
					}
				}
			}
			return {resource};
		}

		virtual con::vector<mem::sptr<ResourceGroup>> CreateResourceGroups(
			con::vector<mem::sptr<ResourceLayout>> layouts) override
		{
			con::vector<mem::sptr<ResourceGroup>> groups{};
			for (mem::sptr<ResourceLayout> layout : layouts)
				groups.emplace_back(CreateResourceGroup(layout));
			return groups;
		}
	};
} //namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_RESOURCE_HPP */