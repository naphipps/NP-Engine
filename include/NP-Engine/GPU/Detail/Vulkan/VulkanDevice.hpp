//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/5/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_DEVICE_HPP
#define NP_ENGINE_GPU_VULKAN_DEVICE_HPP

#include "NP-Engine/Services/Services.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Device.hpp"

#include "VulkanInstance.hpp"
#include "VulkanPresentTarget.hpp"
#include "VulkanLogicalDevice.hpp"
#include "VulkanPipelineCache.hpp"
#include "VulkanFence.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanResult.hpp"

namespace np::gpu::__detail
{
	struct VulkanDeviceMemoryRegion
	{
		siz offset = 0;
		siz size = 0;

		bl IsValid() const
		{
			return size != 0;
		}

		bl Contains(const VulkanDeviceMemoryRegion& other) const
		{
			return IsValid() && other.IsValid() && offset <= other.offset && (offset + size) >= (other.offset + other.size);
		}
	};

	class VulkanDeviceMemory
	{
	private:
		mem::sptr<VulkanLogicalDevice> _device;
		siz _memory_type_index;
		siz _size;
		VkDeviceMemory _memory;
		mutexed_wrapper<void*> _mapping; //for access to memory

		static VkMemoryAllocateInfo CreateVkMemoryAllocateInfo(siz memory_type_index, siz size)
		{
			VkMemoryAllocateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			info.allocationSize = size;
			info.memoryTypeIndex = memory_type_index;
			return info;
		}

		static VkDeviceMemory CreateVkDeviceMemory(mem::sptr<VulkanLogicalDevice> device, siz memory_type_index, siz size)
		{
			mem::sptr<VulkanInstance> instance = device->GetPhysicalDevice().GetDetailInstance();
			VkMemoryAllocateInfo info = CreateVkMemoryAllocateInfo(memory_type_index, size);
			VkDeviceMemory memory = nullptr;
			VkResult result = vkAllocateMemory(*device, &info, instance->GetVulkanAllocationCallbacks(), &memory);
			return result == VK_SUCCESS ? memory : nullptr;
		}

		void* Map(VulkanDeviceMemoryRegion region)
		{
			void* mapping = nullptr;
			VulkanResult result = vkMapMemory(*_device, _memory, region.offset, region.size, 0, &mapping);
			return result.Contains(VulkanResult::Success) ? mapping : nullptr;
		}

		void UnMap()
		{
			vkUnmapMemory(*_device, _memory);
		}

	public:
		VulkanDeviceMemory(mem::sptr<VulkanLogicalDevice> device, siz memory_type_index, siz size) :
			_device(device),
			_memory_type_index(memory_type_index),
			_size(size),
			_memory(CreateVkDeviceMemory(_device, _memory_type_index, _size))
		{}

		~VulkanDeviceMemory()
		{
			if (_memory)
			{
				mem::sptr<VulkanInstance> instance = _device->GetPhysicalDevice().GetDetailInstance();
				vkFreeMemory(*_device, _memory, instance->GetVulkanAllocationCallbacks());
				_memory = nullptr;
			}
		}

		mem::sptr<VulkanLogicalDevice> GetLogicalDevice() const
		{
			return _device;
		}

		mem::sptr<srvc::Services> GetServices() const
		{
			return _device->GetServices();
		}

		siz GetMemoryTypeIndex() const
		{
			return _memory_type_index;
		}

		siz GetSize() const
		{
			return _size;
		}

		siz GetCommitment() const
		{
			VkDeviceSize commitment;
			vkGetDeviceMemoryCommitment(*_device, _memory, &commitment);
			return commitment;
		}

		VulkanResult Bind(VkBuffer buffer, siz offset) const
		{
			return vkBindBufferMemory(*_device, buffer, _memory, offset);
		}

		VulkanResult Bind(VkImage image, siz offset) const
		{
			return vkBindImageMemory(*_device, image, _memory, offset);
		}

		VkMemoryType GetMemoryType() const
		{
			return _device->GetPhysicalDevice().GetVkMemoryProperties().memoryTypes[GetMemoryTypeIndex()];
		}

		VkMemoryHeap GetMemoryHeap() const
		{
			return _device->GetPhysicalDevice().GetVkMemoryProperties().memoryHeaps[GetMemoryType().heapIndex];
		}

		bl IsMappable() const
		{
			return GetMemoryType().propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		}

		bl IsAutoClearCache() const
		{
			return GetMemoryType().propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		}

		bl Contains(VulkanDeviceMemoryRegion region) const
		{
			return region.IsValid() && region.offset < _size && (region.offset + region.size) <= _size;
		}

		bl SetBytes(VulkanDeviceMemoryRegion region, const con::vector<ui8>& bytes)
		{
			bl set = false;
			if (IsMappable() && Contains(region) && !bytes.empty())
			{
				auto mapping = _mapping.get_access();
				*mapping = Map(region);
				if (*mapping)
				{
					const siz byte_count = region.size < bytes.size() ? region.size : bytes.size();
					mem::copy_bytes(*mapping, bytes.data(), byte_count);
					set = true;
					UnMap();
				}
			}
			return set;
		}

		con::vector<ui8> GetBytes(VulkanDeviceMemoryRegion region)
		{
			con::vector<ui8> bytes{};
			if (IsMappable() && Contains(region))
			{
				auto mapping = _mapping.get_access();
				*mapping = Map(region);
				if (*mapping)
				{
					const siz byte_count = region.size < bytes.size() ? region.size : bytes.size();
					bytes.resize(byte_count);
					mem::copy_bytes(bytes.data(), *mapping, bytes.size());
					UnMap();
				}
			}
			return bytes;
		}

		bl ClearCacheForDevice(VulkanDeviceMemoryRegion region)
		{
			VulkanResult result = VulkanResult::None;
			auto mapping = _mapping.get_access();
			*mapping = Map(region);
			if (*mapping)
			{
				if (Contains(region))
				{
					VkMappedMemoryRange range{};
					range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
					range.memory = _memory;
					range.offset = region.offset;
					range.size = region.size;
					result = vkFlushMappedMemoryRanges(*GetLogicalDevice(), 1, &range);
				}
				UnMap();
			}
			return result.Contains(VulkanResult::Success);
		}

		bl ClearCacheForHost(VulkanDeviceMemoryRegion region)
		{
			VulkanResult result = VulkanResult::None;
			auto mapping = _mapping.get_access();
			*mapping = Map(region);
			if (*mapping)
			{
				if (Contains(region))
				{
					VkMappedMemoryRange range{};
					range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
					range.memory = _memory;
					range.offset = region.offset;
					range.size = region.size;
					result = vkInvalidateMappedMemoryRanges(*GetLogicalDevice(), 1, &range);
				}
				UnMap();
			}
			return result.Contains(VulkanResult::Success);
		}
	};

	struct VulkanDeviceMemoryAllocation
	{
	protected:
		mem::sptr<VulkanDeviceMemory> _memory;
		siz _alignment_offset;
		VulkanDeviceMemoryRegion _region;

	public:
		VulkanDeviceMemoryAllocation(mem::sptr<VulkanDeviceMemory> memory, siz alignment_offset, VulkanDeviceMemoryRegion region) :
			_memory(memory),
			_alignment_offset(alignment_offset),
			_region(region)
		{}

		mem::sptr<VulkanDeviceMemory> GetDeviceMemory() const
		{
			return _memory;
		}

		siz GetAlignmentOffset() const
		{
			return _alignment_offset;
		}

		VulkanDeviceMemoryRegion GetRegion() const
		{
			return _region;
		}
	};

	class VulkanDeviceMemoryPool
	{
	protected:
		class VulkanDeviceMemoryAllocationDestroyer : public mem::smart_ptr_contiguous_destroyer<VulkanDeviceMemoryAllocation>
		{
		private:
			using base = mem::smart_ptr_contiguous_destroyer<VulkanDeviceMemoryAllocation>;
			VulkanDeviceMemoryPool& _pool;

		public:
			VulkanDeviceMemoryAllocationDestroyer(VulkanDeviceMemoryPool& pool, mem::allocator& a) :
				base(a),
				_pool(pool)
			{}

			VulkanDeviceMemoryAllocationDestroyer(const VulkanDeviceMemoryAllocationDestroyer& other) :
				base(other._allocator),
				_pool(other._pool)
			{
			}

			VulkanDeviceMemoryAllocationDestroyer(VulkanDeviceMemoryAllocationDestroyer&& other) noexcept :
				base(other._allocator),
				_pool(other._pool)
			{}

			void destruct_object(VulkanDeviceMemoryAllocation* ptr) override
			{
				_pool.ReleaseRegion(ptr->GetAlignmentOffset(), ptr->GetRegion());
				base::destruct_object(ptr);
			}
		};

		mem::sptr<VulkanDeviceMemory> _memory;
		siz _size;
		mutexed_wrapper<con::vector<VulkanDeviceMemoryRegion>> _free_regions;

		void ReleaseRegion(VulkanDeviceMemoryRegion region)
		{
			auto free_regions = _free_regions.get_access();
			if (free_regions->empty())
			{
				free_regions->emplace_back(region);
			}
			else
			{
				for (auto it = free_regions->begin(); it != free_regions->end(); it++)
					if (it->offset > region.offset)
					{
						free_regions->insert(it, region);
						break;
					}
			}

			//coalesce
			for (siz i = 0; i < free_regions->size() - 1;)
			{
				VulkanDeviceMemoryRegion& a = (*free_regions)[i];
				VulkanDeviceMemoryRegion& b = (*free_regions)[i + 1];

				if (a.offset + a.size == b.offset)
				{
					b.offset = a.offset;
					b.size += a.size;
					free_regions->erase(free_regions->begin() + i);
				}
				else
				{
					i++;
				}
			}
		}

		void ReleaseRegion(siz alignment_offset, VulkanDeviceMemoryRegion region)
		{
			region.offset -= alignment_offset;
			region.size += alignment_offset;
			ReleaseRegion(region);
		}

		::std::pair<siz, VulkanDeviceMemoryRegion> AcquireRegion(VkMemoryRequirements requirements, VkMemoryPropertyFlags flags)
		{
			VulkanDeviceMemoryRegion region{};
			siz alignment_offset = 0;

			auto free_regions = _free_regions.get_access();
			//determine best fitting region via index
			siz index = SIZ_MAX;
			for (siz i = 0; i < free_regions->size(); i++)
				if ((*free_regions)[i].size >= requirements.size)
					if (index > free_regions->size() || (*free_regions)[i].size < (*free_regions)[index].size)
						index = i;

			region = index < free_regions->size() ?
				(*free_regions)[index] : VulkanDeviceMemoryRegion{};

			if (region.IsValid())
			{
				//align region
				alignment_offset = mem::is_aligned(region.offset, requirements.alignment) ?
					0 : mem::calc_aligned_value(region.offset, requirements.alignment) - region.offset;

				NP_ENGINE_ASSERT(alignment_offset < region.size,
					"alignment offset must be less than region size so that our region remains valid after alignment");

				region.offset += alignment_offset;
				region.size -= alignment_offset;

				NP_ENGINE_ASSERT(region.size >= requirements.size,
					"aligning region should not reduce region size below required size");

				//split region to reduce as much as reasonable, so we update at index or erase it from free_regions
				if (region.size - requirements.size > mem::DEFAULT_ALIGNMENT) //TODO: default alignment as threshold is arbitrary
				{
					//update at index for split region
					(*free_regions)[index] = { region.offset + requirements.size, region.size - requirements.size };
					region.size = requirements.size;
				}
				else
				{
					free_regions->erase(free_regions->begin() + index);
				}
			}

			return { alignment_offset, region };
		}

	public:
		VulkanDeviceMemoryPool(mem::sptr<VulkanDeviceMemory> memory, siz size) :
			_memory(memory),
			_size(size),
			_free_regions{}
		{
			_free_regions.get_access()->emplace_back(VulkanDeviceMemoryRegion{ 0, _size });
		}

		mem::sptr<VulkanDeviceMemoryAllocation> AllocateDeviceMemory(VkMemoryRequirements requirements, VkMemoryPropertyFlags flags)
		{
			using destroyer_type = VulkanDeviceMemoryAllocationDestroyer;
			using resource_type = mem::smart_ptr_resource<VulkanDeviceMemoryAllocation, destroyer_type>;
			using contiguous_block_type = mem::smart_ptr_contiguous_block<VulkanDeviceMemoryAllocation, resource_type>;

			::std::pair<siz, VulkanDeviceMemoryRegion> acquired = AcquireRegion(requirements, flags);
			resource_type* resource = nullptr;
			if (acquired.second.IsValid())
			{
				mem::allocator& a = GetServices()->GetAllocator();
				contiguous_block_type* contiguous_block = mem::create<contiguous_block_type>(a);

				if (contiguous_block)
				{
					VulkanDeviceMemoryAllocation* object =
						mem::construct<VulkanDeviceMemoryAllocation>(contiguous_block->object_block, _memory, acquired.first, acquired.second);
					resource = mem::construct<resource_type>(contiguous_block->resource_block,
						destroyer_type{ *this, a }, object);
				}
				else
				{
					ReleaseRegion(acquired.first, acquired.second);
				}
			}

			return { resource };
		}

		mem::sptr<srvc::Services> GetServices() const
		{
			return _memory->GetServices();
		}
	};

	class VulkanDeviceMemoryAccumulatingPool
	{
	protected:
		mem::sptr<VulkanLogicalDevice> _device;
		siz _memory_type_index;
		mutexed_wrapper<con::vector<mem::sptr<VulkanDeviceMemoryPool>>> _pools;

	public:
		VulkanDeviceMemoryAccumulatingPool(mem::sptr<VulkanLogicalDevice> device, siz memory_type_index) :
			_device(device),
			_memory_type_index(memory_type_index)
		{}

		mem::sptr<srvc::Services> GetServices() const
		{
			return _device->GetServices();
		}

		siz GetMemoryTypeIndex() const
		{
			return _memory_type_index;
		}

		VkMemoryType GetMemoryType() const
		{
			return _device->GetPhysicalDevice().GetVkMemoryProperties().memoryTypes[GetMemoryTypeIndex()];
		}

		VkMemoryHeap GetMemoryHeap() const
		{
			return _device->GetPhysicalDevice().GetVkMemoryProperties().memoryHeaps[GetMemoryType().heapIndex];
		}

		bl IsCompatible(VkMemoryRequirements requirements, VkMemoryPropertyFlags flags) const
		{
			//TODO: we might want to allow caller to choose which memory type-and-heap directly? we'll think about this...
			return (requirements.memoryTypeBits & BIT(_memory_type_index)) && (GetMemoryType().propertyFlags & flags) == flags;
		}
		
		mem::sptr<VulkanDeviceMemoryAllocation> AllocateDeviceMemory(VkMemoryRequirements requirements, VkMemoryPropertyFlags flags)
		{
			mem::sptr<VulkanDeviceMemoryAllocation> allocation = nullptr;
			if (IsCompatible(requirements, flags))
			{
				auto pools = _pools.get_access();
				for (siz i = 0; !allocation && i < pools->size(); i++)
					allocation = (*pools)[i]->AllocateDeviceMemory(requirements, flags);

				if (!allocation)
				{
					mem::allocator& a = GetServices()->GetAllocator();
					mem::sptr<VulkanDeviceMemory> memory = mem::create_sptr<VulkanDeviceMemory>(a, _device, GetMemoryTypeIndex(), requirements.size);
					siz size = ::std::min(NP_ENGINE_MEM_ACCUMULATING_ALLOCATOR_BLOCK_SIZE, (ui64)(GetMemoryHeap().size * 0.2)); //TODO: 20% of heap is arbitrary
					size = ::std::max(size, requirements.size); //ensure we can at least fit the requirements

					pools->emplace_back(mem::create_sptr<VulkanDeviceMemoryPool>(a, memory, size));
					allocation = pools->back()->AllocateDeviceMemory(requirements, flags);
				}
			}
			return allocation;
		}
	};

	class VulkanDevice : public Device
	{
	private:
		mem::sptr<VulkanPresentTarget> _target;
		VkSurfaceFormatKHR _surface_format;
		VkPresentModeKHR _present_mode;
		mem::sptr<VulkanLogicalDevice> _logical_device;
		con::vector<mem::sptr<VulkanDeviceMemoryAccumulatingPool>> _memory_pools; //one accumulating pool per memory type that we have

		static ::std::optional<VulkanPhysicalDevice> ChoosePhysicalDevice(mem::sptr<VulkanInstance> instance, DeviceUsage usage,
																		  mem::sptr<VulkanPresentTarget> target)
		{
			::std::optional<VulkanPhysicalDevice> device;
			if (target)
			{
				using Candidate = ::std::pair<siz, VulkanPhysicalDevice>; // <score, physical_device>
				con::vector<VulkanPhysicalDevice> devices = VulkanPhysicalDevice::GetAllVulkanPhysicalDevices(instance);
				con::vector<Candidate> candidates;

				for (VulkanPhysicalDevice d : devices)
					candidates.emplace_back(d.GetUsageScore(usage, target), d);

				::std::sort(candidates.begin(), candidates.end(),
							[](const Candidate& a, const Candidate& b)
							{
								return a.first > b.first; // descending sort
							});

				if (!candidates.empty() && candidates.front().first > 0)
					device = candidates.front().second;
			}
			return device;
		}

		static con::vector<VkDeviceQueueCreateInfo> CreateVkQueueCreateInfos(VulkanPhysicalDevice physical_device,
																			 DeviceUsage usage, mem::sptr<PresentTarget> target)
		{
			const con::vector<VkQueueFamilyProperties> properties = physical_device.GetVkQueueFamilyProperties();
			con::oset<ui32> indices{};

			for (siz i = 0; i < properties.size(); i++)
			{
				if (usage.Contains(DeviceUsage::Present) && physical_device.QueueFamilySupportsPresent(properties, i, target))
					indices.emplace((ui32)i);

				if (usage.Contains(DeviceUsage::Graphics) && physical_device.QueueFamilySupportsGraphics(properties, i))
					indices.emplace((ui32)i);

				if (usage.Contains(DeviceUsage::Compute) && physical_device.QueueFamilySupportsCompute(properties, i))
					indices.emplace((ui32)i);
			}

			con::vector<VkDeviceQueueCreateInfo> infos;
			for (ui32 index : indices)
			{
				VkDeviceQueueCreateInfo info = VulkanLogicalDevice::CreateVkDeviceQueueInfo();
				info.queueFamilyIndex = index;
				infos.emplace_back(info);
			}
			return infos;
		}

		static con::vector<mem::sptr<VulkanDeviceMemoryAccumulatingPool>> CreateMemoryPools(mem::sptr<VulkanLogicalDevice> logical_device)
		{
			const VkPhysicalDeviceMemoryProperties properties = logical_device->GetPhysicalDevice().GetVkMemoryProperties();
			con::vector<mem::sptr<VulkanDeviceMemoryAccumulatingPool>> pools(properties.memoryTypeCount);
			for (siz i = 0; i < pools.size(); i++)
				pools[i] = mem::create_sptr<VulkanDeviceMemoryAccumulatingPool>(logical_device->GetServices()->GetAllocator(), logical_device, i);
			return pools;
		}

	public:
		VulkanDevice(mem::sptr<DetailInstance> instance, DeviceUsage usage, mem::sptr<PresentTarget> target):
			_target(target),
			_surface_format(),
			_present_mode(),
			_logical_device(nullptr),
			_memory_pools{}
		{
			mem::sptr<VulkanInstance> vulkan_instance = DetailObject::EnsureIsDetailType(instance, DetailType::Vulkan);
			if (vulkan_instance)
			{
				const ::std::optional<VulkanPhysicalDevice> physical_device_optional =
					ChoosePhysicalDevice(vulkan_instance, usage, _target);
				if (physical_device_optional.has_value())
				{
					const VulkanPhysicalDevice physical_device = physical_device_optional.value();
					_surface_format = physical_device.ChooseVkSurfaceFormat(_target);
					_present_mode = physical_device.ChooseVkPresentMode(_target);
					mem::sptr<srvc::Services> services = physical_device.GetServices();
					_logical_device = mem::create_sptr<VulkanLogicalDevice>(
						services->GetAllocator(), physical_device, CreateVkQueueCreateInfos(physical_device, usage, _target));

					_memory_pools = CreateMemoryPools(_logical_device);
				}
			}
		}

		virtual ~VulkanDevice() = default;

		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _logical_device->GetServices();
		}

		virtual mem::sptr<DetailInstance> GetDetailInstance() const override
		{
			return _logical_device->GetPhysicalDevice().GetDetailInstance();
		}

		virtual mem::sptr<PresentTarget> GetPresentTarget() const override
		{
			return _target;
		}

		virtual con::vector<DeviceQueueFamily> GetDeviceQueueFamilies() const override
		{
			const VulkanPhysicalDevice physical_device = GetLogicalDevice()->GetPhysicalDevice();
			const con::vector<VkQueueFamilyProperties> properties = physical_device.GetVkQueueFamilyProperties();
			con::vector<DeviceQueueFamily> families{};

			for (siz i = 0; i < properties.size(); i++)
			{
				DeviceQueueUsage usage = DeviceQueueUsage::None;

				if (physical_device.QueueFamilySupportsPresent(properties, i, GetPresentTarget()))
					usage |= DeviceQueueUsage::Present;

				if (physical_device.QueueFamilySupportsGraphics(properties, i))
					usage |= DeviceQueueUsage::Graphics;

				if (physical_device.QueueFamilySupportsCompute(properties, i))
					usage |= DeviceQueueUsage::Compute;

				families.emplace_back(DeviceQueueFamily{i, properties[i].queueCount, usage});
			}

			return families;
		}

		virtual mem::sptr<PipelineCache> GetPipelineCache() const override
		{
			return mem::create_sptr<VulkanPipelineCache>(GetServices()->GetAllocator(), GetLogicalDevice(), con::vector<ui8>{});
		}

		virtual mem::sptr<Fence> CreateFence() override
		{
			return mem::create_sptr<VulkanFence>(GetServices()->GetAllocator(), GetLogicalDevice());
		}

		virtual mem::sptr<Semaphore> CreateSemaphore() override
		{
			return mem::create_sptr<VulkanSemaphore>(GetServices()->GetAllocator(), GetLogicalDevice());
		}

		virtual void WaitUntilIdle() const override
		{
			vkDeviceWaitIdle(*GetLogicalDevice());
		}

		mem::sptr<VulkanLogicalDevice> GetLogicalDevice() const
		{
			return _logical_device;
		}

		VkSurfaceFormatKHR GetVkSurfaceFormat() const
		{
			return _surface_format;
		}

		VkPresentModeKHR GetVkPresentMode() const
		{
			return _present_mode;
		}

		mem::sptr<VulkanDeviceMemoryAllocation> AllocateDeviceMemory(VkMemoryRequirements requirements, VkMemoryPropertyFlags flags)
		{
			mem::sptr<VulkanDeviceMemoryAllocation> allocation = nullptr;
			for (siz i = 0; !allocation && i < _memory_pools.size(); i++)
				allocation = _memory_pools[i]->AllocateDeviceMemory(requirements, flags);
			return allocation;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_RENDER_DEVICE_HPP */