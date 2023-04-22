//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/28/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_RENDERABLE_MODEL_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_RENDERABLE_MODEL_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "GraphicsDetailType.hpp"
#include "RenderableObject.hpp"
#include "RenderableMetaValues.hpp"
#include "Model.hpp"

namespace np::gfx
{
	class RenderableModel : public RenderableObject
	{
	protected:
		mem::sptr<srvc::Services> _services;
		mem::sptr<Model> _model;
		RenderableMetaValues _meta_values;
		mem::Delegate _update_meta_values_on_frame; //TODO: I do not think we need this...

		// TODO: add struct that represents all the needed information to render our _model

		RenderableModel(mem::sptr<srvc::Services> services, mem::sptr<Model> model): _services(services), _model(model), _meta_values() {}

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

		RenderableMetaValues& GetMetaValues()
		{
			return _meta_values;
		}

		const RenderableMetaValues& GetMetaValues() const
		{
			return _meta_values;
		}

		void SetMetaValues(const RenderableMetaValues& meta_values)
		{
			_meta_values = meta_values;
		}

		mem::Delegate& GetUpdateMetaValuesOnFrameDelegate()
		{
			return _update_meta_values_on_frame;
		}

		const mem::Delegate& GetUpdateMetaValuesOnFrameDelegate() const
		{
			return _update_meta_values_on_frame;
		}
	};
} // namespace np::gfx

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_RENDERABLE_MODEL_HPP */