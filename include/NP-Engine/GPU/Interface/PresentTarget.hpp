//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/12/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_PRESENT_TARGET_HPP
#define NP_ENGINE_GPU_INTERFACE_PRESENT_TARGET_HPP

#include "NP-Engine/Window/Window.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "Detail.hpp"

// TODO: add ability to render to just half the window/surface, etc -- will be helpful adding DX12/etc support

namespace np::gpu
{
	struct PresentTarget : public DetailObject
	{
		static mem::sptr<PresentTarget> Create(mem::sptr<DetailInstance> instance, mem::sptr<win::Window> window);

		virtual ~PresentTarget() = default;

		virtual mem::sptr<DetailInstance> GetDetailInstance() const = 0;

		virtual mem::sptr<win::Window> GetWindow() const = 0;

		//TODO: I feel like we could benefit from some info we want to use here? maybe this belongs somewhere else? I think we need to be able to pull display info
		//virtual siz GetPixelsPerInch() const = 0; //TODO: this make have to be a device method
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_PRESENT_TARGET_HPP */