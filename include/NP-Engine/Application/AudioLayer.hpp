//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/20/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_AUDIO_LAYER_HPP
#define NP_ENGINE_AUDIO_LAYER_HPP

#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "Layer.hpp"

namespace np::app
{
	class AudioLayer : public Layer
	{
	public:
		AudioLayer(mem::sptr<srvc::Services> services): Layer(services) {}
	};
} // namespace np::app

#endif /* NP_ENGINE_AUDIO_LAYER_HPP */