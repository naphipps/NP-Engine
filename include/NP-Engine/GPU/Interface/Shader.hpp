//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_SHADER_HPP
#define NP_ENGINE_GPU_INTERFACE_SHADER_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "Detail.hpp"
#include "Device.hpp"
#include "Stage.hpp"

namespace np::gpu
{
	struct Shader : public DetailObject
	{
		static mem::sptr<Shader> Create(mem::sptr<Device> device, Stage stage, str filename, str entrypoint);

		virtual ~Shader() = default;

		virtual mem::sptr<Device> GetDevice() const = 0;

		virtual void Load(str filename) = 0;

		virtual void Reload() = 0;

		virtual str GetFilename() const = 0;

		virtual str GetEntrypoint() const = 0;

		virtual void SetEntrypoint(str entrypoint) = 0;

		virtual Stage GetStage() const = 0;

		//virtual void SetValueOverrides() = 0; //TODO: add support for value overrides
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_SHADER_HPP */