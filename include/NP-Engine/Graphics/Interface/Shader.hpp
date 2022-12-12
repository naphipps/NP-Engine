//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_SHADER_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_SHADER_HPP

#include <iostream> //TODO: remove

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"

namespace np::gfx
{
	class Shader
	{
	public:
		enum class Type // TODO: refactor to Pascal case
		{
			VERTEX,
			FRAGMENT,
			TESSELATION_CONTROL,
			TESSELATION_EVALUATION,
			GEOMETRY,
			COMPUTE
		};

	protected:
		Type _type;
		str _filename;
		str _entrypoint;

		str GetShaderStage() const
		{
			str stage = "UNKNOWN";

			switch (_type)
			{
			case Type::VERTEX:
				stage = "vertex";
				break;
			case Type::FRAGMENT:
				stage = "fragment";
				break;
			case Type::TESSELATION_CONTROL:
				stage = "tesscontrol";
				break;
			case Type::TESSELATION_EVALUATION:
				stage = "tesseval";
				break;
			case Type::GEOMETRY:
				stage = "geometry";
				break;
			case Type::COMPUTE:
				stage = "compute";
				break;
			}

			return stage;
		}

	public:
		Shader(str filename, Type type, str entrypoint): _filename(filename), _type(type), _entrypoint(entrypoint) {}

		virtual siz Size() const = 0;

		virtual void* Bytes() const = 0;

		virtual void Load(str filename) = 0;

		virtual void Reload()
		{
			Load(_filename);
		}

		const str& GetFilename() const
		{
			return _filename;
		}

		str& GetFilename()
		{
			return _filename;
		}

		const str& GetEntrypoint() const
		{
			return _entrypoint;
		}

		str& GetEntrypoint()
		{
			return _entrypoint;
		}

		void SetEntrypoint(str entrypoint)
		{
			_entrypoint = entrypoint;
		}

		Type GetType() const
		{
			return _type;
		}
	};
} // namespace np::gfx

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_SHADER_HPP */