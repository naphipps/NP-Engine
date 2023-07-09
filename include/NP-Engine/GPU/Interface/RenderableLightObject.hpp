//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/28/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_RENDERABLE_LIGHT_OBJECT_HPP
#define NP_ENGINE_GPU_INTERFACE_RENDERABLE_LIGHT_OBJECT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "CommandStaging.hpp"
#include "Pipeline.hpp"
#include "RenderableObject.hpp"

namespace np::gpu
{
	class RenderableLightObject : public RenderableObject
	{
	private:
		virtual void Stage(mem::sptr<CommandStaging> command_staging, mem::sptr<Pipeline> pipline) override {
		} // lights do not need this

	public:
		virtual void Stage(mem::sptr<CommandStaging> command_staging, mem::sptr<Pipeline> pipline,
						   mem::sptr<Resource> resource) = 0;
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_RENDERABLE_LIGHT_OBJECT_HPP */