//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_IMAGE_HPP
#define NP_ENGINE_RPI_IMAGE_HPP

#include <utility>

#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/String/String.hpp"

#include "NP-Engine/Vendor/StbInclude.hpp"

#include "Color.hpp"

namespace np::graphics
{
	class Image
	{
	private:
		ui32 _width;
		ui32 _height;
		container::vector<ui8> _pixels;

	public:
		Image(str filename)
		{
			Load(filename);
		}

		Image(ui32 width, ui32 height)
		{
			SetSize(width, height);
		}

		Image(): Image(0, 0) {}

		Image(const Image& other): _width(other._width), _height(other._height), _pixels(other._pixels) {}

		Image(Image&& other) noexcept:
			_width(::std::move(other._width)),
			_height(::std::move(other._height)),
			_pixels(::std::move(other._pixels))
		{}

		~Image() = default; // TODO: do we need this??

		Image& operator=(const Image& other)
		{
			_width = other._width;
			_height = other._height;
			_pixels = other._pixels;
			return *this;
		}

		Image& operator=(Image&& other) noexcept
		{
			_width = ::std::move(other._width);
			_height = ::std::move(other._height);
			_pixels = ::std::move(other._pixels);
			return *this;
		}

		bl Load(str filename) // TODO: add Load from stream and/or memory??
		{
			bl loaded = false;
			i32 x, y, components;
			uchr* image = stbi_load(filename.c_str(), &x, &y, &components, STBI_rgb_alpha);

			if (image != nullptr)
			{
				_width = x;
				_height = y;
				SetSize(_width, _height);

				if (!_pixels.empty())
				{
					memcpy(_pixels.data(), image, _pixels.size());
				}

				stbi_image_free(image);
				loaded = true;
			}
			else
			{
				Clear();
			}

			// TODO: we could log stb's failure reason in an else block here

			return loaded;
		}

		void SetSize(ui32 width, ui32 height)
		{
			_width = width;
			_height = height;

			_pixels.clear();
			_pixels.resize((siz)_width * (siz)_height * sizeof(Color), Color{});
		}

		void Clear()
		{
			SetSize(0, 0);
		}

		ui32 GetWidth() const
		{
			return _width;
		}

		ui32 GetHeight() const
		{
			return _height;
		}

		siz Size() const
		{
			return _pixels.size();
		}

		const ui8* Data() const
		{
			return _pixels.data();
		}

		Color Get(ui32 x, ui32 y) const
		{
			Color pixel{};

			if (x < _width && y < _height)
			{
				const ui8* pixel_data = memory::AddressOf(_pixels[(siz)x + (siz)y * (siz)_width * sizeof(Color)]);
				pixel = *((ui32*)pixel_data);
			}

			return pixel;
		}

		void Set(ui32 x, ui32 y, Color color)
		{
			if (x < _width && y < _height)
			{
				ui8* pixel_data = memory::AddressOf(_pixels[(siz)x + (siz)y * (siz)_width * sizeof(Color)]);
				*((ui32*)pixel_data) = color;
			}
		}
	};
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_IMAGE_HPP */