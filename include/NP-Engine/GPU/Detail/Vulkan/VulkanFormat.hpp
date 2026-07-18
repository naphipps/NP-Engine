//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/6/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_FORMAT_HPP
#define NP_ENGINE_GPU_VULKAN_FORMAT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Format.hpp"

namespace np::gpu::__detail
{
	class VulkanFormat : public Format
	{
	protected:
		static Format ToFormat(VkFormat vk_format)
		{
			Format format = 0;

			switch (vk_format)
			{
			//DEPTH STENCIL
			case VK_FORMAT_D32_SFLOAT_S8_UINT:
				format = { Depth | Stencil | Floating, 1, sizeof(ui32) };
				break;
			case VK_FORMAT_D24_UNORM_S8_UINT:
				format = { Depth | Stencil | Unsigned, 1, sizeof(ui8) * 3 };
				break;
			case VK_FORMAT_D16_UNORM_S8_UINT:
				format = { Depth | Stencil | Unsigned, 1, sizeof(ui16) };
				break;
			case VK_FORMAT_S8_UINT:
				format = { Stencil, 1, sizeof(ui8) };
				break;

			//DEPTH
			case VK_FORMAT_D32_SFLOAT:
				format = { Depth | Floating, 1, sizeof(ui32) };
				break;
			case VK_FORMAT_X8_D24_UNORM_PACK32:
				format = { Depth | Unsigned | OptionalByte, 1, sizeof(ui8) * 3 };
				break;
			case VK_FORMAT_D16_UNORM:
				format = { Depth | Unsigned, 1, sizeof(ui16) };
				break;

			//UNSIGNED INT
			case VK_FORMAT_R64G64B64A64_UINT:
				format = {Integer | Unsigned, 4, sizeof(ui64)};
				break;
			case VK_FORMAT_R32G32B32A32_UINT:
				format = {Integer | Unsigned, 4, sizeof(ui32)};
				break;
			case VK_FORMAT_R16G16B16A16_UINT:
				format = {Integer | Unsigned, 4, sizeof(ui16)};
				break;
			case VK_FORMAT_B8G8R8A8_SRGB:
				format = {Integer | Unsigned | SwapBlueRed | GammaCorrection, 4, sizeof(ui8)};
				break;
			case VK_FORMAT_R8G8B8A8_SRGB:
				format = {Integer | Unsigned | GammaCorrection, 4, sizeof(ui8)};
				break;
			case VK_FORMAT_B8G8R8A8_UINT:
				format = {Integer | Unsigned | SwapBlueRed, 4, sizeof(ui8)};
				break;
			case VK_FORMAT_R8G8B8A8_UINT:
				format = {Integer | Unsigned, 4, sizeof(ui8)};
				break;

			case VK_FORMAT_R64G64B64_UINT:
				format = {Integer | Unsigned, 3, sizeof(ui64)};
				break;
			case VK_FORMAT_R32G32B32_UINT:
				format = {Integer | Unsigned, 3, sizeof(ui32)};
				break;
			case VK_FORMAT_R16G16B16_UINT:
				format = {Integer | Unsigned, 3, sizeof(ui16)};
				break;
			case VK_FORMAT_B8G8R8_SRGB:
				format = {Integer | Unsigned | SwapBlueRed | GammaCorrection, 3, sizeof(ui8)};
				break;
			case VK_FORMAT_R8G8B8_SRGB:
				format = {Integer | Unsigned | GammaCorrection, 3, sizeof(ui8)};
				break;
			case VK_FORMAT_B8G8R8_UINT:
				format = {Integer | Unsigned | SwapBlueRed, 3, sizeof(ui8)};
				break;
			case VK_FORMAT_R8G8B8_UINT:
				format = {Integer | Unsigned, 3, sizeof(ui8)};
				break;

			case VK_FORMAT_R64G64_UINT:
				format = {Integer | Unsigned, 2, sizeof(ui64)};
				break;
			case VK_FORMAT_R32G32_UINT:
				format = {Integer | Unsigned, 2, sizeof(ui32)};
				break;
			case VK_FORMAT_R16G16_UINT:
				format = {Integer | Unsigned, 2, sizeof(ui16)};
				break;
			case VK_FORMAT_R8G8_SRGB:
				format = {Integer | Unsigned | GammaCorrection, 2, sizeof(ui8)};
				break;
			case VK_FORMAT_R8G8_UINT:
				format = {Integer | Unsigned, 2, sizeof(ui8)};
				break;

			case VK_FORMAT_R64_UINT:
				format = {Integer | Unsigned, 1, sizeof(ui64)};
				break;
			case VK_FORMAT_R32_UINT:
				format = {Integer | Unsigned, 1, sizeof(ui32)};
				break;
			case VK_FORMAT_R16_UINT:
				format = {Integer | Unsigned, 1, sizeof(ui16)};
				break;
			case VK_FORMAT_R8_SRGB:
				format = {Integer | Unsigned | GammaCorrection, 1, sizeof(ui8)};
				break;
			case VK_FORMAT_R8_UINT:
				format = {Integer | Unsigned, 1, sizeof(ui8)};
				break;

			// SIGNED INT
			case VK_FORMAT_R64G64B64A64_SINT:
				format = {Integer, 4, sizeof(i64)};
				break;
			case VK_FORMAT_R32G32B32A32_SINT:
				format = {Integer, 4, sizeof(i32)};
				break;
			case VK_FORMAT_R16G16B16A16_SINT:
				format = {Integer, 4, sizeof(i16)};
				break;
			case VK_FORMAT_B8G8R8A8_SINT:
				format = {Integer | SwapBlueRed, 4, sizeof(i8)};
				break;
			case VK_FORMAT_R8G8B8A8_SINT:
				format = {Integer, 4, sizeof(i8)};
				break;

			case VK_FORMAT_R64G64B64_SINT:
				format = {Integer, 3, sizeof(i64)};
				break;
			case VK_FORMAT_R32G32B32_SINT:
				format = {Integer, 3, sizeof(i32)};
				break;
			case VK_FORMAT_R16G16B16_SINT:
				format = {Integer, 3, sizeof(i16)};
				break;
			case VK_FORMAT_B8G8R8_SINT:
				format = {Integer | SwapBlueRed, 3, sizeof(i8)};
				break;
			case VK_FORMAT_R8G8B8_SINT:
				format = {Integer, 3, sizeof(i8)};
				break;

			case VK_FORMAT_R64G64_SINT:
				format = {Integer, 2, sizeof(i64)};
				break;
			case VK_FORMAT_R32G32_SINT:
				format = {Integer, 2, sizeof(i32)};
				break;
			case VK_FORMAT_R16G16_SINT:
				format = {Integer, 2, sizeof(i16)};
				break;
			case VK_FORMAT_R8G8_SINT:
				format = {Integer, 2, sizeof(i8)};
				break;

			case VK_FORMAT_R64_SINT:
				format = {Integer, 1, sizeof(i64)};
				break;
			case VK_FORMAT_R32_SINT:
				format = {Integer, 1, sizeof(i32)};
				break;
			case VK_FORMAT_R16_SINT:
				format = {Integer, 1, sizeof(i16)};
				break;
			case VK_FORMAT_R8_SINT:
				format = {Integer, 1, sizeof(i8)};
				break;

			//SFLOAT
			case VK_FORMAT_R64G64B64A64_SFLOAT:
				format = {Floating, 4, sizeof(dbl)};
				break;
			case VK_FORMAT_R32G32B32A32_SFLOAT:
				format = {Floating, 4, sizeof(flt)};
				break;
			case VK_FORMAT_R16G16B16A16_SFLOAT:
				format = {Floating, 4, sizeof(flt) / 2}; //TODO: cpp23 has float16_t
				break;

			case VK_FORMAT_R64G64B64_SFLOAT:
				format = {Floating, 3, sizeof(dbl)};
				break;
			case VK_FORMAT_R32G32B32_SFLOAT:
				format = {Floating, 3, sizeof(flt)};
				break;
			case VK_FORMAT_R16G16B16_SFLOAT:
				format = {Floating, 3, sizeof(flt) / 2}; //TODO: cpp23 has float16_t
				break;

			case VK_FORMAT_R64G64_SFLOAT:
				format = {Floating, 2, sizeof(dbl)};
				break;
			case VK_FORMAT_R32G32_SFLOAT:
				format = {Floating, 2, sizeof(flt)};
				break;
			case VK_FORMAT_R16G16_SFLOAT:
				format = {Floating, 2, sizeof(flt) / 2}; //TODO: cpp23 has float16_t
				break;

			case VK_FORMAT_R64_SFLOAT:
				format = {Floating, 1, sizeof(dbl)};
				break;
			case VK_FORMAT_R32_SFLOAT:
				format = {Floating, 1, sizeof(flt)};
				break;
			case VK_FORMAT_R16_SFLOAT:
				format = {Floating, 1, sizeof(flt) / 2}; //TODO: cpp23 has float16_t
				break;
			};

			return format;
		}

	public:
		VulkanFormat(ui32 value, ui32 component_count, ui32 component_size): Format(value, component_count, component_size) {}

		VulkanFormat(ui32 value): Format(value) {}

		VulkanFormat(VkFormat vk_format): Format(ToFormat(vk_format)) {}

		//TODO: add constructor for Vk Format Features?

		VkFormat GetVkFormat() const
		{
			const ui32 count = GetComponentCount();
			const ui32 size = GetComponentSize();
			VkFormat format = VK_FORMAT_UNDEFINED;

			if (!Contains(Features))
			{
				//DEPTH STENCIL
				if (Contains(Depth | Stencil | Floating) && count == 1 && size == 4)
					format = VK_FORMAT_D32_SFLOAT_S8_UINT;
				else if (Contains(Depth | Stencil | Unsigned) && count == 1 && size == 3)
					format = VK_FORMAT_D24_UNORM_S8_UINT;
				else if (Contains(Depth | Stencil | Unsigned) && count == 1 && size == 2)
					format = VK_FORMAT_D16_UNORM_S8_UINT;
				else if (Equals(Stencil) || (Contains(Stencil) && count == 1 && size == 1))
					format = VK_FORMAT_S8_UINT;

				//DEPTH
				else if (Contains(Depth | Floating) && count == 1 && size == 4)
					format = VK_FORMAT_D32_SFLOAT;
				else if (Contains(Depth | Unsigned | OptionalByte) && count == 1 && size == 3)
					format = VK_FORMAT_X8_D24_UNORM_PACK32;
				else if (Contains(Depth | Unsigned) && count == 1 && size == 2)
					format = VK_FORMAT_D16_UNORM;

				//UNSIGNED INT
				else if (Contains(Integer | Unsigned) && count == 4 && size == 8)
					format = VK_FORMAT_R64G64B64A64_UINT;
				else if (Contains(Integer | Unsigned) && count == 4 && size == 4)
					format = VK_FORMAT_R32G32B32A32_UINT;
				else if (Contains(Integer | Unsigned) && count == 4 && size == 2)
					format = VK_FORMAT_R16G16B16A16_UINT;
				else if (Contains(Integer | Unsigned | SwapBlueRed | GammaCorrection) && count == 4 && size == 1)
					format = VK_FORMAT_B8G8R8A8_SRGB;
				else if (Contains(Integer | Unsigned | GammaCorrection) && count == 4 && size == 1)
					format = VK_FORMAT_R8G8B8A8_SRGB;
				else if (Contains(Integer | Unsigned | SwapBlueRed) && count == 4 && size == 1)
					format = VK_FORMAT_B8G8R8A8_UINT;
				else if (Contains(Integer | Unsigned) && count == 4 && size == 1)
					format = VK_FORMAT_R8G8B8A8_UINT;

				else if (Contains(Integer | Unsigned) && count == 3 && size == 8)
					format = VK_FORMAT_R64G64B64_UINT;
				else if (Contains(Integer | Unsigned) && count == 3 && size == 4)
					format = VK_FORMAT_R32G32B32_UINT;
				else if (Contains(Integer | Unsigned) && count == 3 && size == 2)
					format = VK_FORMAT_R16G16B16_UINT;
				else if (Contains(Integer | Unsigned | SwapBlueRed | GammaCorrection) && count == 3 && size == 1)
					format = VK_FORMAT_B8G8R8_SRGB;
				else if (Contains(Integer | Unsigned | GammaCorrection) && count == 3 && size == 1)
					format = VK_FORMAT_R8G8B8_SRGB;
				else if (Contains(Integer | Unsigned | SwapBlueRed) && count == 3 && size == 1)
					format = VK_FORMAT_B8G8R8_UINT;
				else if (Contains(Integer | Unsigned) && count == 3 && size == 1)
					format = VK_FORMAT_R8G8B8_UINT;

				else if (Contains(Integer | Unsigned) && count == 2 && size == 8)
					format = VK_FORMAT_R64G64_UINT;
				else if (Contains(Integer | Unsigned) && count == 2 && size == 4)
					format = VK_FORMAT_R32G32_UINT;
				else if (Contains(Integer | Unsigned) && count == 2 && size == 2)
					format = VK_FORMAT_R16G16_UINT;
				else if (Contains(Integer | Unsigned | GammaCorrection) && count == 2 && size == 1)
					format = VK_FORMAT_R8G8_SRGB;
				else if (Contains(Integer | Unsigned) && count == 2 && size == 1)
					format = VK_FORMAT_R8G8_UINT;

				else if (Contains(Integer | Unsigned) && count == 1 && size == 8)
					format = VK_FORMAT_R64_UINT;
				else if (Contains(Integer | Unsigned) && count == 1 && size == 4)
					format = VK_FORMAT_R32_UINT;
				else if (Contains(Integer | Unsigned) && count == 1 && size == 2)
					format = VK_FORMAT_R16_UINT;
				else if (Contains(Integer | Unsigned | GammaCorrection) && count == 1 && size == 1)
					format = VK_FORMAT_R8_SRGB;
				else if (Contains(Integer | Unsigned) && count == 1 && size == 1)
					format = VK_FORMAT_R8_UINT;

				// SIGNED INT
				else if (Contains(Integer) && count == 4 && size == 8)
					format = VK_FORMAT_R64G64B64A64_SINT;
				else if (Contains(Integer) && count == 4 && size == 4)
					format = VK_FORMAT_R32G32B32A32_SINT;
				else if (Contains(Integer) && count == 4 && size == 2)
					format = VK_FORMAT_R16G16B16A16_SINT;
				else if (Contains(Integer | SwapBlueRed) && count == 4 && size == 1)
					format = VK_FORMAT_B8G8R8A8_SINT;
				else if (Contains(Integer) && count == 4 && size == 1)
					format = VK_FORMAT_R8G8B8A8_SINT;

				else if (Contains(Integer) && count == 3 && size == 8)
					format = VK_FORMAT_R64G64B64_SINT;
				else if (Contains(Integer) && count == 3 && size == 4)
					format = VK_FORMAT_R32G32B32_SINT;
				else if (Contains(Integer) && count == 3 && size == 2)
					format = VK_FORMAT_R16G16B16_SINT;
				else if (Contains(Integer | SwapBlueRed) && count == 3 && size == 1)
					format = VK_FORMAT_B8G8R8_SINT;
				else if (Contains(Integer) && count == 3 && size == 1)
					format = VK_FORMAT_R8G8B8_SINT;

				else if (Contains(Integer) && count == 2 && size == 8)
					format = VK_FORMAT_R64G64_SINT;
				else if (Contains(Integer) && count == 2 && size == 4)
					format = VK_FORMAT_R32G32_SINT;
				else if (Contains(Integer) && count == 2 && size == 2)
					format = VK_FORMAT_R16G16_SINT;
				else if (Contains(Integer) && count == 2 && size == 1)
					format = VK_FORMAT_R8G8_SINT;

				else if (Contains(Integer) && count == 1 && size == 8)
					format = VK_FORMAT_R64_SINT;
				else if (Contains(Integer) && count == 1 && size == 4)
					format = VK_FORMAT_R32_SINT;
				else if (Contains(Integer) && count == 1 && size == 2)
					format = VK_FORMAT_R16_SINT;
				else if (Contains(Integer) && count == 1 && size == 1)
					format = VK_FORMAT_R8_SINT;

				//SFLOAT
				else if (Contains(Floating) && count == 4 && size == 8)
					format = VK_FORMAT_R64G64B64A64_SFLOAT;
				else if (Contains(Floating) && count == 4 && size == 4)
					format = VK_FORMAT_R32G32B32A32_SFLOAT;
				else if (Contains(Floating) && count == 4 && size == 2)
					format = VK_FORMAT_R16G16B16A16_SFLOAT;

				else if (Contains(Floating) && count == 3 && size == 8)
					format = VK_FORMAT_R64G64B64_SFLOAT;
				else if (Contains(Floating) && count == 3 && size == 4)
					format = VK_FORMAT_R32G32B32_SFLOAT;
				else if (Contains(Floating) && count == 3 && size == 2)
					format = VK_FORMAT_R16G16B16_SFLOAT;

				else if (Contains(Floating) && count == 2 && size == 8)
					format = VK_FORMAT_R64G64_SFLOAT;
				else if (Contains(Floating) && count == 2 && size == 4)
					format = VK_FORMAT_R32G32_SFLOAT;
				else if (Contains(Floating) && count == 2 && size == 2)
					format = VK_FORMAT_R16G16_SFLOAT;

				else if (Contains(Floating) && count == 1 && size == 8)
					format = VK_FORMAT_R64_SFLOAT;
				else if (Contains(Floating) && count == 1 && size == 4)
					format = VK_FORMAT_R32_SFLOAT;
				else if (Contains(Floating) && count == 1 && size == 2)
					format = VK_FORMAT_R16_SFLOAT;
			}

			return format;
		}

		VkFormatFeatureFlags GetVkFormatFeatureFlags() const
		{
			/*
				TODO: add values to represent format features
				- [ ] VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT = 0x00000004,
				- [ ] VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT = 0x00000020,
				- [ ] VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT = 0x00001000,
			*/
			
			VkFormatFeatureFlags flags = 0;

			if (Contains(Features))
			{
				const bl contains_read = Contains(FeatureRead);
				const bl contains_write = Contains(FeatureWrite);

				if (Contains(FeatureDepth | FeatureStencil))
					flags |= VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
				if (Contains(FeatureSampled | FeatureImage))
					flags |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
				if (Contains(FeatureStorage | FeatureImage))
					flags |= VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
				if (Contains(FeatureUniform | FeatureTexel | FeatureBuffer))
					flags |= VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
				if (Contains(FeatureStorage | FeatureTexel | FeatureBuffer))
					flags |= VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
				if (Contains(FeatureVertex | FeatureBuffer))
					flags |= VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
				
				if (Contains(FeatureBlit))
				{
					if (contains_read || !contains_write)
						flags |= VK_FORMAT_FEATURE_BLIT_SRC_BIT;
					if (contains_write || !contains_read)
						flags |= VK_FORMAT_FEATURE_BLIT_DST_BIT;
				}

				if (Contains(FeatureTransfer))
				{
					if (contains_read || !contains_write)
						flags |= VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
					if (contains_write || !contains_read)
						flags |= VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
				}

				if (Contains(FeatureColor | FeatureBlend))
					flags |= VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
				if (Contains(FeatureColor))
					flags |= VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
			}

			return flags;
		}
	};

} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_FORMAT_HPP */