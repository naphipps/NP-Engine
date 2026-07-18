//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 6/8/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_RESOURCE_GROUP_HPP
#define NP_ENGINE_GPU_VULKAN_RESOURCE_GROUP_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/ResourceGroup.hpp"

#include "VulkanImageResourceView.hpp"
#include "VulkanSamplerResource.hpp"
#include "VulkanBufferResource.hpp"

namespace np::gpu::__detail
{
	struct VulkanResourceAssignment
	{
		ui32 resourceDescriptionIndex;

		VulkanResourceAssignment(const ResourceAssignment& other = {}):
			resourceDescriptionIndex(other.resourceDescriptionIndex)
		{}

		operator ResourceAssignment() const
		{
			return { resourceDescriptionIndex };
		}

		VkWriteDescriptorSet GetVkWriteDescriptorSet() const
		{
			VkWriteDescriptorSet writer{};
			writer.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writer.dstBinding = resourceDescriptionIndex;
			return writer;
		}
	};

	struct VulkanImageResourceAssignmentContext
	{
		mem::sptr<VulkanImageResourceView> view;
		mem::sptr<VulkanSamplerResource> sampler;
		VulkanImageResourceUsage usage;

		VulkanImageResourceAssignmentContext(const ImageResourceAssignmentContext& other = {}) :
			view((other.view && other.view->GetDetailType() == DetailType::Vulkan) ? other.view : nullptr),
			sampler((other.sampler && other.sampler->GetDetailType() == DetailType::Vulkan) ? other.sampler : nullptr),
			usage(other.usage)
		{}

		operator ImageResourceAssignmentContext() const
		{
			return { view, sampler, usage };
		}

		VkDescriptorImageInfo GetVkDescriptorImageInfo() const
		{
			return
			{
				sampler ? *sampler : static_cast<VkSampler>(nullptr),
				view ? *view : static_cast<VkImageView>(nullptr),
				VulkanImageResourceUsage{usage}.GetVkImageLayout()
			};
		}
	};

	struct VulkanBufferResourceAssignmentContext
	{
		mem::sptr<VulkanBufferResource> buffer;
		siz offset;
		siz range;

		VulkanBufferResourceAssignmentContext(const BufferResourceAssignmentContext& other = {}) :
			buffer((other.buffer && other.buffer->GetDetailType() == DetailType::Vulkan) ? other.buffer : nullptr),
			offset(other.offset),
			range(other.range)
		{}

		operator BufferResourceAssignmentContext() const
		{
			return { buffer, offset, range };
		}

		VkDescriptorBufferInfo GetVkDescriptorBufferInfo() const
		{
			return { buffer ? *buffer : static_cast<VkBuffer>(nullptr), offset, range };
		}
	};

	struct VulkanImageResourceAssignment : public VulkanResourceAssignment
	{
		con::vector<VulkanImageResourceAssignmentContext> contexts;

		VulkanImageResourceAssignment(const ImageResourceAssignment& other = {}) :
			VulkanResourceAssignment(other),
			contexts{ other.contexts.begin(), other.contexts.end() }
		{}

		operator ImageResourceAssignment() const
		{
			return { resourceDescriptionIndex, { contexts.begin(), contexts.end() } };
		}

		VkWriteDescriptorSet GetVkWriteDescriptorSet() const
		{
			VkWriteDescriptorSet writer = VulkanResourceAssignment::GetVkWriteDescriptorSet();
			writer.descriptorCount = contexts.size();
			return writer;
		}

		con::vector<VkDescriptorImageInfo> GetVkDescriptorImageInfos() const
		{
			con::vector<VkDescriptorImageInfo> infos(contexts.size());
			for (siz i = 0; i < infos.size(); i++)
				infos[i] = contexts[i].GetVkDescriptorImageInfo();
			return infos;
		}

		virtual bl IsCompatible(const ResourceDescription& description) const
		{
			return description.count == contexts.size() && description.type.Contains(ResourceType::Image);
		}
	};

	struct VulkanBufferResourceAssignment : public VulkanResourceAssignment
	{
		con::vector<VulkanBufferResourceAssignmentContext> contexts;

		VulkanBufferResourceAssignment(const BufferResourceAssignment& other = {}) :
			VulkanResourceAssignment(other),
			contexts{ other.contexts.begin(), other.contexts.end() }
		{}

		operator BufferResourceAssignment() const
		{
			return { resourceDescriptionIndex, { contexts.begin(), contexts.end() } };
		}

		VkWriteDescriptorSet GetVkWriteDescriptorSet() const
		{
			VkWriteDescriptorSet writer = VulkanResourceAssignment::GetVkWriteDescriptorSet();
			writer.descriptorCount = contexts.size();
			return writer;
		}

		con::vector<VkDescriptorBufferInfo> GetVkDescriptorBufferInfos() const
		{
			con::vector<VkDescriptorBufferInfo> infos(contexts.size());
			for (siz i = 0; i < infos.size(); i++)
				infos[i] = contexts[i].GetVkDescriptorBufferInfo();
			return infos;
		}

		virtual bl IsCompatible(const ResourceDescription& description) const
		{
			return description.count == contexts.size() && description.type.Contains(ResourceType::Buffer);
		}
	};

	struct VulkanCopyResourceAssignment
	{
		mem::sptr<ResourceGroup> dstResourceGroup;
		ui32 dstResourceDescriptionIndex;
		mem::sptr<ResourceGroup> srcResourceGroup;
		ui32 srcResourceDescriptionIndex;
		ui32 resourceCount;

		VulkanCopyResourceAssignment(const CopyResourceAssignment& other = {}) :
			dstResourceGroup(other.dstResourceGroup),
			dstResourceDescriptionIndex(other.dstResourceDescriptionIndex),
			srcResourceGroup(other.srcResourceGroup),
			srcResourceDescriptionIndex(other.srcResourceDescriptionIndex),
			resourceCount(other.resourceCount)
		{}

		operator CopyResourceAssignment() const
		{
			return { dstResourceGroup, dstResourceDescriptionIndex, srcResourceGroup, srcResourceDescriptionIndex };
		}

		/*
			Caller sets VkCopyDescriptorSet.dstSet AND VkCopyDescriptorSet.srcSet
		*/
		VkCopyDescriptorSet GetVkCopyDescriptorSet() const
		{
			VkCopyDescriptorSet copy{};
			copy.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
			copy.dstBinding = dstResourceDescriptionIndex;
			//copy.dstArrayElement = 0;//TODO: support image arrays
			copy.srcBinding = srcResourceDescriptionIndex;
			//copy.srcArrayElement = 0;//TODO: support image arrays
			copy.descriptorCount = resourceCount;
			return copy;
		}
	};

	struct VulkanResourceAssignments
	{
		con::vector<VulkanImageResourceAssignment> imageAssignments;
		con::vector<VulkanBufferResourceAssignment> bufferAssignments;
		con::vector<VulkanCopyResourceAssignment> copyAssignments;

		VulkanResourceAssignments(const ResourceAssignments& other = {}):
			imageAssignments{other.imageAssignments.begin(), other.imageAssignments.end()},
			bufferAssignments{ other.bufferAssignments.begin(), other.bufferAssignments.end() },
			copyAssignments{other.copyAssignments.begin(), other.copyAssignments.end()}
		{}

		operator ResourceAssignments() const
		{
			return
			{
				{imageAssignments.begin(), imageAssignments.end()},
				{bufferAssignments.begin(), bufferAssignments.end()},
				{copyAssignments.begin(), copyAssignments.end()}
			};
		}

		con::vector<con::vector<VkDescriptorImageInfo>> GetVkDescriptorImageInfos() const
		{
			con::vector<con::vector<VkDescriptorImageInfo>> infos(imageAssignments.size());
			for (siz i = 0; i < infos.size(); i++)
				infos[i] = imageAssignments[i].GetVkDescriptorImageInfos();
			return infos;
		}

		con::vector<con::vector<VkDescriptorBufferInfo>> GetVkDescriptorBufferInfos() const
		{
			con::vector<con::vector<VkDescriptorBufferInfo>> infos(bufferAssignments.size());
			for (siz i = 0; i < infos.size(); i++)
				infos[i] = bufferAssignments[i].GetVkDescriptorBufferInfos();
			return infos;
		}

		con::vector<VkCopyDescriptorSet> GetVkCopyDescriptorSets() const
		{
			con::vector<VkCopyDescriptorSet> infos(copyAssignments.size());
			for (siz i = 0; i < infos.size(); i++)
				infos[i] = copyAssignments[i].GetVkCopyDescriptorSet();
			return infos;
		}
	};

	class VulkanResourceGroup : public ResourceGroup
	{
	protected:
		mem::sptr<VulkanResourceLayout> _resource_layout;
		VkDescriptorSet _set;

	public:
		VulkanResourceGroup(mem::sptr<ResourceLayout> resource_layout, VkDescriptorSet set) :
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

		virtual Result ApplyResourceAssignments(const ResourceAssignments& assignments_) override
		{
			const VulkanResourceAssignments assignments{ assignments_ };
			const con::vector<ResourceDescription> descriptions = _resource_layout->GetResourceDescriptions();
			VulkanResult result = VulkanResult::None;
			bl is_compatible = true;

			for (auto it = assignments.imageAssignments.begin(); is_compatible && it != assignments.imageAssignments.end(); it++)
			{
				is_compatible &= it->resourceDescriptionIndex < descriptions.size();
				is_compatible &= is_compatible && it->IsCompatible(descriptions[it->resourceDescriptionIndex]);
			}

			for (auto it = assignments.bufferAssignments.begin(); is_compatible && it != assignments.bufferAssignments.end(); it++)
			{
				is_compatible &= it->resourceDescriptionIndex < descriptions.size();
				is_compatible &= is_compatible && it->IsCompatible(descriptions[it->resourceDescriptionIndex]);
			}

			for (auto it = assignments.copyAssignments.begin(); is_compatible && it != assignments.copyAssignments.end(); it++)
			{
				mem::sptr<ResourceLayout> dst_resource_layout = it->dstResourceGroup->GetResourceLayout();
				mem::sptr<ResourceLayout> src_resource_layout = it->srcResourceGroup->GetResourceLayout();
				const con::vector<ResourceDescription> dst_descriptions = dst_resource_layout->GetResourceDescriptions();
				const con::vector<ResourceDescription> src_descriptions = src_resource_layout->GetResourceDescriptions();
				is_compatible &= it->dstResourceDescriptionIndex < dst_descriptions.size();
				is_compatible &= it->srcResourceDescriptionIndex < src_descriptions.size();

				if (is_compatible)
				{
					const ResourceDescription& dst_description = dst_descriptions[it->dstResourceDescriptionIndex];
					const ResourceDescription& src_description = src_descriptions[it->srcResourceDescriptionIndex];

					is_compatible &= dst_description.IsCompatible(src_description);
					is_compatible &= it->resourceCount <= dst_description.count;
					is_compatible &= it->resourceCount <= src_description.count;
				}
			}

			if (!is_compatible)
			{
				result = VulkanResult::Error;
			}
			else
			{
				mem::sptr<VulkanDevice> device = _resource_layout->GetDevice();
				mem::sptr<VulkanLogicalDevice> logical_device = device->GetLogicalDevice();
				VulkanPhysicalDevice physical_device = logical_device->GetPhysicalDevice();
				VkPhysicalDeviceProperties properties = physical_device.GetVkProperties();
				VkPhysicalDeviceLimits& limits = properties.limits;

				con::vector<con::vector<VkDescriptorImageInfo>> image_infos = assignments.GetVkDescriptorImageInfos();
				con::vector<con::vector<VkDescriptorBufferInfo>> buffer_infos = assignments.GetVkDescriptorBufferInfos();
				con::vector<VkWriteDescriptorSet> writers(assignments.imageAssignments.size() + assignments.bufferAssignments.size());
				con::vector<VkCopyDescriptorSet> copiers = assignments.GetVkCopyDescriptorSets();
				
				siz writer_offset = 0;
				for (siz i = 0; i < assignments.imageAssignments.size(); i++)
				{
					//TODO: support array images
					const VulkanImageResourceAssignment& assignment = assignments.imageAssignments[i];
					VkWriteDescriptorSet& writer = writers[writer_offset + i];
					writer = assignment.GetVkWriteDescriptorSet();
					writer.pImageInfo = image_infos[i].data();
				}

				writer_offset = assignments.imageAssignments.size();
				for (siz i = 0; i < assignments.bufferAssignments.size(); i++)
				{
					const VulkanBufferResourceAssignment& assignment = assignments.bufferAssignments[i];
					VkDescriptorBufferInfo* info = buffer_infos[i].data();
					VkWriteDescriptorSet& writer = writers[writer_offset + i];
					writer = assignment.GetVkWriteDescriptorSet();
					writer.pBufferInfo = info;
					const VulkanResourceDescription& description = descriptions[writer.dstBinding];

					if (description.type.Contains(VulkanBufferResourceUsage::Uniform))
					{
						info->offset = mem::calc_aligned_value(info->offset, limits.minUniformBufferOffsetAlignment);
						info->range = ::std::min(info->range, (VkDeviceSize)limits.maxUniformBufferRange);
					}

					if (description.type.Contains(VulkanBufferResourceUsage::Storage))
					{
						info->offset = mem::calc_aligned_value(info->offset, limits.minStorageBufferOffsetAlignment);
						info->range = ::std::min(info->range, (VkDeviceSize)limits.maxStorageBufferRange);
					}
				}

				for (siz i = 0; i < writers.size(); i++)
				{
					writers[i].dstSet = _set;
					writers[i].descriptorType = VulkanResourceDescription{ descriptions[writers[i].dstBinding] }.GetVkDescriptorType();
				}

				for (siz i = 0; i < copiers.size(); i++)
				{
					mem::sptr<VulkanResourceGroup> dst = DetailObject::EnsureIsDetailType(assignments.copyAssignments[i].dstResourceGroup, DetailType::Vulkan);
					mem::sptr<VulkanResourceGroup> src = DetailObject::EnsureIsDetailType(assignments.copyAssignments[i].srcResourceGroup, DetailType::Vulkan);
					copiers[i].dstSet = *dst;
					copiers[i].srcSet = *src;
				}

				vkUpdateDescriptorSets(*logical_device,
					writers.size(), writers.empty() ? nullptr : writers.data(),
					copiers.size(), copiers.empty() ? nullptr : copiers.data());

				result = VulkanResult::Success;
			}

			return result;
		}

		virtual bl IsCompatible(mem::sptr<ResourceGroup> other) const override
		{
			return IsCompatible(other->GetResourceLayout());
		}

		virtual bl IsCompatible(mem::sptr<ResourceLayout> layout) const override
		{
			return _resource_layout->IsCompatible(layout);
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
			VulkanResourceGroupDestroyer(mem::sptr<VulkanDevice> device, VkDescriptorPool pool, mem::allocator& a) :
				base(a),
				_device(device),
				_pool(pool)
			{}

			VulkanResourceGroupDestroyer(const VulkanResourceGroupDestroyer& other) :
				base(other._allocator),
				_device(other._device),
				_pool(other._pool)
			{}

			VulkanResourceGroupDestroyer(VulkanResourceGroupDestroyer&& other) noexcept :
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
				vkFreeDescriptorSets(*_device->GetLogicalDevice(), _pool, 1, &set);
				//TODO: ^ do we do anything with this result? Other than an assert, I can't imagine what we could do with it
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
					pool_sizes.emplace_back(VkDescriptorPoolSize{ type, 1 });
			}

			VkDescriptorPoolCreateInfo info = CreateVkInfo(size);
			info.poolSizeCount = pool_sizes.size();
			info.pPoolSizes = pool_sizes.empty() ? nullptr : pool_sizes.data();
			info.flags |= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; //enforce

			mem::sptr<VulkanInstance> instance = device->GetDetailInstance();
			VkDescriptorPool pool = nullptr;
			VulkanResult result = vkCreateDescriptorPool(*device->GetLogicalDevice(), &info, instance->GetVulkanAllocationCallbacks(), &pool);
			return result.Contains(VulkanResult::Success) ? pool : nullptr;
		}

	public:
		VulkanResourceGroupPool(mem::sptr<Device> device, siz size, const con::vector<ResourceDescription>& descriptions) :
			_device(device),
			_size(size),
			_descriptions(descriptions.begin(), descriptions.end()),
			_pool(CreateVkDescriptorPool(_device, _size, _descriptions))
		{}

		virtual ~VulkanResourceGroupPool()
		{
			if (_pool)
			{
				mem::sptr<VulkanInstance> instance = _device->GetDetailInstance();
				vkDestroyDescriptorPool(*_device->GetLogicalDevice(), _pool, instance->GetVulkanAllocationCallbacks());
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
					VulkanResult result = vkAllocateDescriptorSets(*_device->GetLogicalDevice(), &info, &set);

					if (result.Contains(VulkanResult::Success))
					{
						VulkanResourceGroup* object =
							mem::construct<VulkanResourceGroup>(contiguous_block->object_block, vulkan_layout, set);
						resource = mem::construct<resource_type>(contiguous_block->resource_block,
							destroyer_type{ _device, _pool, a }, object);
					}
				}
			}
			return { resource };
		}

		virtual con::vector<mem::sptr<ResourceGroup>> CreateResourceGroups(
			const con::vector<mem::sptr<ResourceLayout>>& layouts) override
		{
			con::vector<mem::sptr<ResourceGroup>> groups{};
			for (mem::sptr<ResourceLayout> layout : layouts)
				groups.emplace_back(CreateResourceGroup(layout));
			return groups;
		}
	};
} //namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_RESOURCE_HPP */