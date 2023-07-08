//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/28/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_RENDERABLE_MODEL_HPP
#define NP_ENGINE_GPU_INTERFACE_RENDERABLE_MODEL_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "DetailType.hpp"
#include "RenderableObject.hpp"
#include "Model.hpp"

namespace np::gpu
{
	class RenderableModel : public RenderableObject
	{
	protected:
		mem::sptr<srvc::Services> _services;
		mem::sptr<Model> _model;

		// TODO: add struct that represents all the needed information to render our _model

		RenderableModel(mem::sptr<srvc::Services> services, mem::sptr<Model> model): _services(services), _model(model)
		{
			_meta.resourceType = (ui32)GetType();
		}

	public:
		virtual ~RenderableModel() {}

		virtual ResourceType GetType() const override
		{
			return ResourceType::RenderableModel;
		}

		mem::sptr<Model> GetModel() const
		{
			return _model;
		}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_RENDERABLE_MODEL_HPP */