//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_TEXTURE_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_TEXTURE_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"

#include "Image.hpp"

namespace np::gfx
{
	class Texture : public Image
	{
	private:
		bl _hot_reloadable;

	public:
		Texture(bl hot_reloadable = false): Image(), _hot_reloadable(hot_reloadable) {}

		Texture(str image_filepath, bl hot_reloadable = false): Image(image_filepath), _hot_reloadable(hot_reloadable) {}

		Texture(const Image& image, bl hot_reloadable = false): Image(image), _hot_reloadable(hot_reloadable) {}

		Texture(Image&& image, bl hot_reloadable = false): Image(::std::move(image)), _hot_reloadable(hot_reloadable) {}

		Texture(const Texture& other): Image(other), _hot_reloadable(other._hot_reloadable) {}

		Texture(Texture&& other) noexcept: Image(::std::move(other)), _hot_reloadable(::std::move(other._hot_reloadable)) {}

		Texture& operator=(const Texture& other)
		{
			Image::operator=(other);
			_hot_reloadable = other._hot_reloadable;
			return *this;
		}

		Texture& operator=(Texture&& other) noexcept
		{
			Image::operator=(other);
			_hot_reloadable = ::std::move(other._hot_reloadable);
			return *this;
		}

		void Clear() override
		{
			Image::Clear();
			_hot_reloadable = false;
		}

		virtual void SetHotReloadable(bl hot_reloadable = true)
		{
			_hot_reloadable = hot_reloadable;
		}

		bl IsHotReloadable() const
		{
			return _hot_reloadable;
		}
	};
} // namespace np::gfx

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_TEXTURE_HPP */