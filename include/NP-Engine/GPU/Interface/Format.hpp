//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/6/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_FORMAT_HPP
#define NP_ENGINE_GPU_INTERFACE_FORMAT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::gpu
{
	class Format : public enm_ui32
	{
	protected:
		//store component count in bits [16, 23]
		constexpr static ui32 ComponentCountShift = 16;
		constexpr static ui32 ComponentCountMask = BIT(24) - BIT(ComponentCountShift);

		//store depth in bits [24, 31]
		constexpr static ui32 ComponentSizeShift = 24;
		constexpr static ui32 ComponentSizeMask = BIT(32) - BIT(ComponentSizeShift);

	public:
		constexpr static ui32 Integer = BIT(0);
		constexpr static ui32 Floating = BIT(1);
		/*
			used as "Unsigned Normalized" for depth/stencil formats
		*/
		constexpr static ui32 Unsigned = BIT(2);

		//constexpr static ui32 AlphaFirst = BIT(...); //TODO: investigate other formats
		constexpr static ui32 OptionalByte = BIT(10);
		constexpr static ui32 SwapBlueRed = BIT(11);
		constexpr static ui32 Stencil = BIT(12);
		constexpr static ui32 Depth = BIT(13);
		constexpr static ui32 GammaCorrection = BIT(14);
		/*
			use this to indicate this format is to describe features and not component count/size
		*/
		constexpr static ui32 Features = BIT(15);

		/*
			TODO: add values to represent format features
			- [ ] VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT = 0x00000004,
			- [ ] VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT = 0x00000020,
			- [ ] VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT = 0x00001000,
		*/

		constexpr static ui32 FeatureDepth = Depth;
		constexpr static ui32 FeatureStencil = Stencil;

		constexpr static ui32 FeatureRead = BIT(16);
		constexpr static ui32 FeatureWrite = BIT(17);

		constexpr static ui32 FeatureBuffer = BIT(18);
		constexpr static ui32 FeatureImage = BIT(19);

		constexpr static ui32 FeatureTransfer = BIT(20);
		constexpr static ui32 FeatureBlit = BIT(21);
		constexpr static ui32 FeatureBlend = BIT(22);
		constexpr static ui32 FeatureVertex = BIT(23);
		constexpr static ui32 FeatureStorage = BIT(24);
		constexpr static ui32 FeatureSampled = BIT(25);
		constexpr static ui32 FeatureUniform = BIT(26);
		constexpr static ui32 FeatureTexel = BIT(27);
		constexpr static ui32 FeatureColor = BIT(28);
		
		Format(ui32 value, ui32 component_count, ui32 component_size): enm_ui32(value)
		{
			SetComponentCount(component_count);
			SetComponentSize(component_size);
		}

		Format(ui32 value): enm_ui32(value) {}

		ui32 GetComponentSize() const
		{
			return Contains(Features) ? 0 : get_embedded_value(ComponentSizeMask, ComponentSizeShift);
		}

		/*
			only accepting values expressed by eight bits
			common values: sizeof(ui8), sizeof(ui16), sizeof(ui32), sizeof(ui64), sizeof(siz), sizeof(flt), sizeof(dbl)
		*/
		void SetComponentSize(ui32 size)
		{
			if (!Contains(Features))
				set_embedded_value(size, ComponentSizeMask, ComponentSizeShift);
		}

		ui32 GetComponentCount() const
		{
			return Contains(Features) ? 0 : get_embedded_value(ComponentCountMask, ComponentCountShift);
		}

		/*
			only accepting values expressed by eight bits
		*/
		void SetComponentCount(ui32 count)
		{
			if (!Contains(Features))
				set_embedded_value(count, ComponentCountMask, ComponentCountShift);
		}
	};

} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_FORMAT_HPP */