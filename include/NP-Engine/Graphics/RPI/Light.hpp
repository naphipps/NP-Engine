//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_LIGHT_HPP
#define NP_ENGINE_RPI_LIGHT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

#include "Color.hpp"

namespace np::graphics
{
	class Light
	{
	public:
		enum class Type
		{
			Omnidirectional,
			Point
		};

	protected:
		Type _type = Type::Omnidirectional;
		flt _radius;
		Color _color;
		::glm::vec3 _position;

	public:
		// TODO: put fields representing lights

		// TODO: add move constructor

		virtual Type GetType() const
		{
			return _type;
		}
	};
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_LIGHT_HPP */