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
	class Format : public Enum<ui32>
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

		//constexpr static ui32 AlphaFirst = BIT(10); //TODO: investigate other formats
		constexpr static ui32 SwapBlueRed = BIT(11);
		constexpr static ui32 Stencil = BIT(12);
		constexpr static ui32 Depth = BIT(13);
		constexpr static ui32 Unsigned = BIT(14);
		constexpr static ui32 GammaCorrection = BIT(15);

		Format(ui32 value, ui32 component_count, ui32 component_size): Enum<ui32>(value)
		{
			SetComponentCount(component_count);
			SetComponentSize(component_size);
		}

		Format(ui32 value): Enum<ui32>(value) {}

		virtual ui32 GetComponentSize() const
		{
			return GetEmbeddedValue(ComponentSizeMask, ComponentSizeShift);
		}

		/*
			only accepting values expressed by eight bits
			common values: sizeof(ui8), sizeof(ui16), sizeof(ui32), sizeof(ui64), sizeof(siz), sizeof(flt), sizeof(dbl)
		*/
		virtual void SetComponentSize(ui32 size)
		{
			SetEmbeddedValue(size, ComponentSizeMask, ComponentSizeShift);
		}

		virtual ui32 GetComponentCount() const
		{
			return GetEmbeddedValue(ComponentCountMask, ComponentCountShift);
		}

		/*
			only accepting values expressed by eight bits
		*/
		virtual void SetComponentCount(ui32 count)
		{
			SetEmbeddedValue(count, ComponentCountMask, ComponentCountShift);
		}
	};

} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_FORMAT_HPP */