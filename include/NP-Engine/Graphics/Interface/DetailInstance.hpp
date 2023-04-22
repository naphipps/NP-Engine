//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/12/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_DETAIL_INSTANCE_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_DETAIL_INSTANCE_HPP

#include "NP-Engine/Services/Services.hpp"

#include "GraphicsDetailType.hpp"

namespace np::gfx
{
	class DetailInstance
	{
	protected:
		mem::sptr<srvc::Services> _services;

		DetailInstance(mem::sptr<srvc::Services> services): _services(services) {}

	public:
		static mem::sptr<DetailInstance> Create(GraphicsDetailType detail_type, mem::sptr<srvc::Services> services);

		virtual GraphicsDetailType GetDetailType() const
		{
			return GraphicsDetailType::None;
		}

		virtual mem::sptr<srvc::Services> GetServices() const
		{
			return _services;
		}
	};
}

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_DETAIL_INSTANCE_HPP */