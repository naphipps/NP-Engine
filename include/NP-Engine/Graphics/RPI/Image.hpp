//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_IMAGE_HPP
#define NP_ENGINE_RPI_IMAGE_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/String/String.hpp"

#include "NP-Engine/Vendor/LodePngInclude.hpp"

#include "Color.hpp"

namespace np::graphics
{
	class Image
	{
	private:
		ui32 _width;
		ui32 _height;
		container::vector<ui8> _pixels;
		str _filename;

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

		Image(const Image& other):
			_width(other._width),
			_height(other._height),
			_pixels(other._pixels),
			_filename(other._filename)
		{}

		Image(Image&& other) noexcept:
			_width(::std::move(other._width)),
			_height(::std::move(other._height)),
			_pixels(::std::move(other._pixels)),
			_filename(::std::move(other._filename))
		{
			other.Clear();
		}

		Image& operator=(const Image& other)
		{
			_width = other._width;
			_height = other._height;
			_pixels = other._pixels;
			_filename = other._filename;
			return *this;
		}

		Image& operator=(Image&& other) noexcept
		{
			_width = ::std::move(other._width);
			_height = ::std::move(other._height);
			_pixels = ::std::move(other._pixels);
			_filename = ::std::move(other._filename);
			other.Clear();
			return *this;
		}

		bl Load(str filename) // TODO: add Load from stream and/or memory??
		{
			_filename = filename;

			// TODO: is there a way for us to pass our _pixels in?
			::std::vector<ui8> pixels;
			ui32 error_code = ::lodepng::decode(pixels, _width, _height, _filename.c_str());
			_pixels.assign(::std::make_move_iterator(pixels.begin()), ::std::make_move_iterator(pixels.end()));

			if (error_code)
			{
				Clear();
				NP_ENGINE_ASSERT(!error_code,
								 "Image did not load '" + _filename + "', LODEPNG REASON: '" +
									 str(lodepng_error_text(error_code)) + "'\n");
			}

			return error_code == 0;
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
			_filename.clear();
		}

		str GetFilename() const
		{
			return _filename;
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