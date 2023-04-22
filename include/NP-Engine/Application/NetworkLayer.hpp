//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/20/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NETWORK_LAYER_HPP
#define NP_ENGINE_NETWORK_LAYER_HPP

#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "Layer.hpp"

namespace np::app
{
	class NetworkLayer : public Layer
	{
	public:
		NetworkLayer(mem::sptr<srvc::Services> services) : Layer(services) {}
	};
}

#endif /* NP_ENGINE_NETWORK_LAYER_HPP */