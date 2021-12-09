//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_SHADER_HPP
#define NP_ENGINE_RPI_SHADER_HPP

#include <iostream> //TODO: remove

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"

namespace np::graphics
{
	class Shader
	{
	protected:
		str _filename;

	public:
		Shader(str filename): _filename(filename) {}

		virtual void Load(str filename) = 0;

		virtual void Reload()
		{
			Load(_filename);
		}

		str Filename() const
		{
			return _filename;
		}

		virtual siz Size() const = 0;

		virtual void* Bytes() const = 0;
	};
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_SHADER_HPP */