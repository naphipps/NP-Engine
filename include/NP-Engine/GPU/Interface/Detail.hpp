//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/12/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_DETAIL_HPP
#define NP_ENGINE_GPU_INTERFACE_DETAIL_HPP

#include "NP-Engine/Services/Services.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

namespace np::gpu
{
	enum class DetailType : ui32
	{
		None = 0,
		Vulkan,
		OpenGL,
		DirectX,
		Metal
	};

	class DetailObject
	{
	protected:
		//TODO: I feel like we ought to have these scattered everywhere just to ensure things
		static mem::sptr<DetailObject> EnsureIsDetailType(mem::sptr<DetailObject> object, DetailType type) 
		{
			return (object && object->GetDetailType() == type) ? object : nullptr;
		}

	public:
		virtual ~DetailObject() = default;

		virtual DetailType GetDetailType() const = 0;

		/*
			this is an optional override
		*/
		virtual mem::sptr<srvc::Services> GetServices() const
		{
			return nullptr;
		}
	};

	//TODO: use this for those detail objects that we want services with, then remove DetailObject::GetServices
	struct DetailObjectWithServices : public DetailObject
	{
		virtual ~DetailObjectWithServices() = default;

		virtual mem::sptr<srvc::Services> GetServices() const = 0;
	};

	struct DetailInstance : public DetailObject
	{
		static mem::sptr<DetailInstance> Create(DetailType type, mem::sptr<srvc::Services> services);

		virtual ~DetailInstance() = default;
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_DETAIL_HPP */