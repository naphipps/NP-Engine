//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/28/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_COLOR_HPP
#define NP_ENGINE_GPU_INTERFACE_COLOR_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Insight/Insight.hpp"

// TODO: add static colors

namespace np::gpu
{
	class ColorChannel : public Enum<ui32>
	{
	public:
		constexpr static ui32 R = BIT(0);
		constexpr static ui32 G = BIT(1);
		constexpr static ui32 B = BIT(2);
		constexpr static ui32 A = BIT(3);

		constexpr static ui32 Red = R;
		constexpr static ui32 Green = G;
		constexpr static ui32 Blue = B;
		constexpr static ui32 Alpha = A;

		ColorChannel(ui32 value): Enum<ui32>(value) {}

		bl IsSingleChannel() const
		{
			return ContainsAll(R) || ContainsAll(G) || ContainsAll(B) || ContainsAll(A);
		}
	};

	/*
		defaults to opaque black
	*/
	struct Color
	{
		union {
			ui8 r = UI8_MIN;
			ui8 Red;
		};

		union {
			ui8 g = UI8_MIN;
			ui8 Green;
		};

		union {
			ui8 b = UI8_MIN;
			ui8 Blue;
		};

		union {
			ui8 a = UI8_MAX;
			ui8 Alpha;
		};

		operator ui32() const
		{
			ui32 i = UI32_MIN;
			mem::CopyBytes(&i, &r, sizeof(ui32));
			return i;
		}

		Color& operator=(ui32 i)
		{
			r = (i >> 24) & UI8_MAX;
			g = (i >> 16) & UI8_MAX;
			b = (i >> 8) & UI8_MAX;
			a = i & UI8_MAX;
			return *this;
		}

		ui8 GetChannelValue(ColorChannel channel) const
		{
			ui8 value = UI8_MIN;
			switch (channel)
			{
			case ColorChannel::R:
				value = r;
				break;
			case ColorChannel::G:
				value = g;
				break;
			case ColorChannel::B:
				value = b;
				break;
			case ColorChannel::A:
				value = a;
				break;
			}
			return value;
		}
	};

	struct ClearColor
	{
		Color color{};
		dbl depth = 0;
		siz stencil = 0;
	};

	NP_ENGINE_STATIC_ASSERT(sizeof(Color) == sizeof(ui32), "Color must have size of ui32 (aka: 4).");
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_COLOR_HPP */