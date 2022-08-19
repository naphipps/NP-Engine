//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/28/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_RENDERABLE_MODEL_HPP
#define NP_ENGINE_RPI_RENDERABLE_MODEL_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "RhiType.hpp"
#include "RenderableObject.hpp"
#include "RenderableType.hpp"
#include "RenderableMetaValues.hpp"
#include "Model.hpp"

namespace np::graphics
{
	class RenderableModel : public RenderableObject
	{
	protected:
		services::Services& _services;
		Model& _model;
		RenderableMetaValues _meta_values;
		memory::Delegate _update_meta_values_on_frame;

		// TODO: add struct that represents all the needed information to render our _model

		RenderableModel(services::Services& services, Model& model): _services(services), _model(model), _meta_values() {}

	public:
		static RenderableModel* Create(services::Services& services, Model& model);

		virtual ~RenderableModel() {}

		virtual RenderableType GetType() const override
		{
			return RenderableType::Model;
		}

		Model& GetModel()
		{
			return _model;
		}

		const Model& GetModel() const
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

		memory::Delegate& GetUpdateMetaValuesOnFrameDelegate()
		{
			return _update_meta_values_on_frame;
		}

		const memory::Delegate& GetUpdateMetaValuesOnFrameDelegate() const
		{
			return _update_meta_values_on_frame;
		}
	};
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_RENDERABLE_MODEL_HPP */