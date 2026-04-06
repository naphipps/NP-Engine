//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/4/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_SAMPLER_RESOURCE_HPP
#define NP_ENGINE_GPU_VULKAN_SAMPLER_RESOURCE_HPP

#include "NP-Engine/Services/Services.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/SamplerResource.hpp"

#include "VulkanDevice.hpp"
#include "VulkanCompareOperation.hpp"
#include "VulkanResource.hpp"

namespace np::gpu::__detail
{
	class VulkanSamplerBorder : public SamplerBorder
	{
	public:
		VulkanSamplerBorder(ui32 value): SamplerBorder(value) {}

		VkBorderColor GetBorderColor() const
		{
			VkBorderColor color = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;

			if (ContainsAll(Transparent | Black))
				color = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
			else if (ContainsAll(Opaque | Black))
				color = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			else if (ContainsAll(Opaque | White))
				color = VK_BORDER_COLOR_INT_OPAQUE_WHITE;

			return color;
		}
	};

	class VulkanSamplerAddressMode : public SamplerAddressMode
	{
	public:
		VulkanSamplerAddressMode(ui32 value): SamplerAddressMode(value) {}

		VkSamplerAddressMode GetSamplerAddressMode() const
		{
			VkSamplerAddressMode mode = VK_SAMPLER_ADDRESS_MODE_REPEAT;

			if (ContainsAll(Repeat))
				mode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			else if (ContainsAll(Mirror | Repeat))
				mode = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			else if (ContainsAll(Clamp | Edge))
				mode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			else if (ContainsAll(Clamp | Border))
				mode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			else if (ContainsAll(Mirror | Clamp | Edge))
				mode = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;

			return mode;
		}
	};

	struct VulkanSamplerAddressModes
	{
		VulkanSamplerAddressMode u = VulkanSamplerAddressMode::None;
		VulkanSamplerAddressMode v = VulkanSamplerAddressMode::None;
		VulkanSamplerAddressMode w = VulkanSamplerAddressMode::None;

		VulkanSamplerAddressModes(const SamplerAddressModes& modes = {}): u(modes.u), v(modes.v), w(modes.w) {}

		operator SamplerAddressModes() const
		{
			return {u, v, w};
		}
	};

	class VulkanSamplerResourceUsage : public SamplerResourceUsage
	{
	public:
		VulkanSamplerResourceUsage(ui32 value): SamplerResourceUsage(value) {}

		VkFilter GetMagnifyFilter() const
		{
			return Contains(MagnifyNearest) ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;
		}

		VkFilter GetMinimizeFilter() const
		{
			return Contains(MinimizeNearest) ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;
		}

		VkSamplerMipmapMode GetMipmapMode() const
		{
			return Contains(MipmapNearest) ? VK_SAMPLER_MIPMAP_MODE_NEAREST : VK_SAMPLER_MIPMAP_MODE_LINEAR;
		}

		bl UseUnnormalizeCoordinates() const
		{
			return Contains(UnnormalizedCoordinates);
		}
	};

	struct VulkanLodBounds
	{
		flt min = 0;
		flt max = 0;

		VulkanLodBounds(const LodBounds& other = {}): min(other.min), max(other.max) {}

		operator LodBounds() const //TODO: I like this -- we should do this for all the vulkan-ized objects we have
		{
			return {min, max};
		}
	};

	class VulkanSamplerResource : public SamplerResource
	{
	private:
		mem::sptr<VulkanDevice> _device;
		flt _anisotrophy;
		VulkanCompareOperation _op;
		VulkanLodBounds _lod_bounds;
		VulkanSamplerBorder _border;
		VulkanSamplerAddressModes _address_modes;
		VkSampler _sampler;

		static VkSamplerCreateInfo CreateVkInfo(VulkanSamplerResourceUsage usage, flt anisotrophy, VulkanCompareOperation op,
												VulkanLodBounds lod_bounds, VulkanSamplerBorder border,
												VulkanSamplerAddressModes address_modes)
		{
			VkSamplerCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			info.addressModeU = address_modes.u.GetSamplerAddressMode();
			info.addressModeV = address_modes.v.GetSamplerAddressMode();
			info.addressModeW = address_modes.w.GetSamplerAddressMode();
			info.unnormalizedCoordinates = usage.UseUnnormalizeCoordinates() ? VK_TRUE : VK_FALSE;
			info.borderColor = border.GetBorderColor();
			info.magFilter = usage.GetMagnifyFilter();
			info.minFilter = usage.GetMinimizeFilter();
			info.compareEnable = op.ContainsAny(VulkanCompareOperation::All) ? VK_TRUE : VK_FALSE;
			info.compareOp = op.GetVkCompareOp();
			info.mipmapMode = usage.GetMipmapMode();
			info.mipLodBias = 0.0f;
			info.minLod = lod_bounds.min;
			info.maxLod = lod_bounds.max;
			info.maxAnisotropy = anisotrophy;

			info.anisotropyEnable = info.maxAnisotropy > 1.f ? VK_TRUE : VK_FALSE;
			return info;
		}

		static VkSampler CreateVkSampler(mem::sptr<VulkanDevice> device, VulkanSamplerResourceUsage usage, flt anisotrophy,
										 VulkanCompareOperation op, VulkanLodBounds lod_bounds, VulkanSamplerBorder border,
										 VulkanSamplerAddressModes address_modes)
		{
			mem::sptr<VulkanLogicalDevice> logical_device = device->GetLogicalDevice();
			const VulkanPhysicalDevice physical_device = logical_device->GetPhysicalDevice();
			const VkPhysicalDeviceProperties properties = physical_device.GetVkProperties();
			const VkPhysicalDeviceFeatures features = physical_device.GetVkFeatures();
			anisotrophy = features.samplerAnisotropy == VK_TRUE
				? ::std::clamp(anisotrophy, 1.f, properties.limits.maxSamplerAnisotropy)
				: 1.f;

			VkSamplerCreateInfo info = CreateVkInfo(usage, anisotrophy, op, lod_bounds, border, address_modes);

			mem::sptr<VulkanInstance> instance = physical_device.GetDetailInstance();
			VkSampler sampler = nullptr;
			VkResult result = vkCreateSampler(*logical_device, &info, instance->GetVulkanAllocationCallbacks(), &sampler);
			return result == VK_SUCCESS ? sampler : nullptr;
		}

	public:
		VulkanSamplerResource(mem::sptr<Device> device, SamplerResourceUsage usage, dbl anisotrophy, CompareOperation op,
							  LodBounds lod_bounds, SamplerBorder border, SamplerAddressModes address_modes):
			_device(device),
			_anisotrophy(anisotrophy),
			_op(op),
			_lod_bounds(lod_bounds),
			_border(border),
			_address_modes(address_modes),
			_sampler(CreateVkSampler(_device, VulkanSamplerResourceUsage{usage}, _anisotrophy, _op, _lod_bounds, _border,
									 _address_modes))
		{}

		~VulkanSamplerResource()
		{
			if (_sampler)
			{
				mem::sptr<VulkanInstance> instance = _device->GetDetailInstance();
				vkDestroySampler(*_device->GetLogicalDevice(), _sampler, instance->GetVulkanAllocationCallbacks());
				_sampler = nullptr;
			}
		}

		operator VkSampler() const
		{
			return _sampler;
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

		virtual dbl GetAnisotrophy() const override
		{
			return _anisotrophy;
		}

		virtual CompareOperation GetCompareOperation() const override
		{
			return _op;
		}

		virtual LodBounds GetLodBounds() const override
		{
			return _lod_bounds;
		}

		virtual SamplerBorder GetBorder() const override
		{
			return _border;
		}

		virtual SamplerAddressModes GetAddressModes() const override
		{
			return _address_modes;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_SAMPLER_RESOURCE_HPP */