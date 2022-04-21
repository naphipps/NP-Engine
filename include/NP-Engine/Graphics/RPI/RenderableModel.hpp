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

#include "RhiType.hpp"
#include "RenderableObject.hpp"
#include "Model.hpp"

namespace np::graphics
{
	class RenderableModel : public RenderableObject
	{
	protected:
		Model& _model;

		RenderableModel(Model& model): _model(model) {}

	public:
		static RenderableModel* Create(memory::Allocator& allocator, Model& model);

		Model& GetModel()
		{
			return _model;
		}

		const Model& GetModel() const
		{
			return _model;
		}
	};
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_RENDERABLE_MODEL_HPP */