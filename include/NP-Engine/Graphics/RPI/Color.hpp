//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/28/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_COLOR_HPP
#define NP_ENGINE_RPI_COLOR_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Insight/Insight.hpp"

namespace np::graphics
{
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
			ui8 a = 0;
			ui8 Alpha;
		};

		operator ui32() const
		{
			ui32 i = 0;
			memory::CopyBytes(&i, &r, sizeof(ui32));
			return i;
		}

		Color& operator=(ui32 i)
		{
			r = (i >> 24) & 255;
			g = (i >> 16) & 255;
			b = (i >> 8) & 255;
			a = (i)&255;
			return *this;
		}
	};

	NP_STATIC_ASSERT(sizeof(Color) == 4, "Color must have size equaling 4.");
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_COLOR_HPP */