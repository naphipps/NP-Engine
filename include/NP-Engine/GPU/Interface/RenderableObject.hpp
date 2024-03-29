//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_RENDERABLE_OBJECT_HPP
#define NP_ENGINE_GPU_INTERFACE_RENDERABLE_OBJECT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "CommandStaging.hpp"
#include "Pipeline.hpp"
#include "Resource.hpp"
#include "RenderableMetaValues.hpp"

namespace np::gpu
{
	class RenderableObject : public Resource
	{
	protected:
		RenderableMetaValues _meta;

	public:
		virtual ~RenderableObject() = default;

		virtual RenderableMetaValues& GetMetaValues()
		{
			return _meta;
		}

		virtual const RenderableMetaValues& GetMetaValues() const
		{
			return _meta;
		}

		virtual void SetMetaValues(const RenderableMetaValues& meta)
		{
			_meta = meta;
		}

		virtual ResourceType GetType() const override = 0;

		virtual void Stage(mem::sptr<CommandStaging> command_staging, mem::sptr<Pipeline> pipline) = 0;

		virtual void PrepareForPipeline(mem::sptr<Pipeline> pipeline) = 0;

		virtual void DisposeForPipeline(mem::sptr<Pipeline> pipeline) = 0;

		virtual bl IsOutOfDate() const = 0;

		virtual void SetOutOfDate(bl is_out_of_date = true) = 0;
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_RENDERABLE_OBJECT_HPP */