//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/28/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_COLOR_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_COLOR_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Insight/Insight.hpp"

// TODO: I think we could use some static colors

namespace np::gpu
{
	enum class ColorChannel : ui32
	{
		None = 0,
		Red = BIT(1),
		Green = BIT(2),
		Blue = BIT(3),
		Alpha = BIT(4),
		All = Red | Green | Blue | Alpha
	};

	constexpr static bl ColorChannelMatches(ColorChannel a, ColorChannel b)
	{
		return a == b;
	}

	constexpr static bl ColorChannelContains(ColorChannel a, ColorChannel b)
	{
		return ((ui64)a & (ui64)b) != (ui64)ColorChannel::None;
	}

	constexpr static bl IsOnlyOneColorChannel(ColorChannel a)
	{
		return a == ColorChannel::Red || a == ColorChannel::Green || a == ColorChannel::Blue || a == ColorChannel::Alpha;
	}

	struct Color
	{
		union {
			ui8 r = 0;
			ui8 Red;
		};

		union {
			ui8 g = 0;
			ui8 Green;
		};

		union {
			ui8 b = 0;
			ui8 Blue;
		};

		union {
			ui8 a = 255;
			ui8 Alpha;
		};

		operator ui32() const
		{
			ui32 i = 0;
			mem::CopyBytes(&i, &r, sizeof(ui32));
			return i;
		}

		Color& operator=(ui32 i)
		{
			r = (i >> 24) & 255;
			g = (i >> 16) & 255;
			b = (i >> 8) & 255;
			a = i & 255;
			return *this;
		}

		ui8 GetChannelValue(ColorChannel channel) const
		{
			ui8 value = 0;
			switch (channel)
			{
			case ColorChannel::Red:
				value = Red;
				break;
			case ColorChannel::Green:
				value = Green;
				break;
			case ColorChannel::Blue:
				value = Blue;
				break;
			case ColorChannel::Alpha:
				value = Alpha;
				break;
			default:
				NP_ENGINE_ASSERT(false, "Only one color channel can be used in GetChannel");
			}

			return value;
		}
	};

	NP_ENGINE_STATIC_ASSERT(sizeof(Color) == 4, "Color must have size of 4.");
} // namespace np::gpu

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_COLOR_HPP */