//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_SHADER_HPP
#define NP_ENGINE_GPU_INTERFACE_SHADER_HPP

#include <iostream> //TODO: remove

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"

#include "DetailType.hpp"

namespace np::gpu
{
	class Shader //TODO: we need to break this up to RenderShader, ComputeShader, etc, so that we can put the correct device with the correct shader
	{
	public:
		enum class Type // TODO: refactor to Pascal case
		{
			Vertex,
			Fragment,
			Compute,
			TesselationControl,
			TesselationEvaluation,
			Geometry
		};

		struct Properties
		{
			Type type;
			str filename;
			str entrypoint; //TODO: default to "main"?
		};

	protected:
		Properties _properties;

		str GetShaderStage() const
		{
			str stage = "UNKNOWN";

			switch (_properties.type)
			{
			case Type::Vertex:
				stage = "vertex";
				break;
			case Type::Fragment:
				stage = "fragment";
				break;
			case Type::TesselationControl:
				stage = "tesscontrol";
				break;
			case Type::TesselationEvaluation:
				stage = "tesseval";
				break;
			case Type::Geometry:
				stage = "geometry";
				break;
			case Type::Compute:
				stage = "compute";
				break;
			}

			return stage;
		}

		Shader(Properties& properties) : _properties(properties) {}

	public:

		virtual DetailType GetDetailType() const = 0;
		
		virtual siz Size() const = 0;

		virtual void* Bytes() const = 0;

		virtual void Load(str filename) = 0;

		virtual void Reload()
		{
			Load(_properties.filename);
		}

		str GetFilename() const
		{
			return _properties.filename;
		}

		str GetEntrypoint() const
		{
			return _properties.entrypoint;
		}

		const chr* GetEntrypointCStr() const
		{
			return _properties.entrypoint.c_str();
		}

		void SetEntrypoint(str entrypoint)
		{
			_properties.entrypoint = entrypoint;
		}

		Type GetType() const
		{
			return _properties.type;
		}

		Properties& GetProperties()
		{
			return _properties;
		}

		const Properties& GetProperties() const
		{
			return _properties;
		}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_SHADER_HPP */