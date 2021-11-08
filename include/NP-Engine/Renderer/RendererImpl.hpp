//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/12/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RENDERER_IMPL_HPP
#define NP_ENGINE_RENDERER_IMPL_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/System/System.hpp"

#include "RPI/Renderer.hpp"

namespace np::renderer
{
    //TODO: add summary comments

    class Renderer
    {
    private:

        static container::vector<rpi::Renderer*> _renderers;

    public:

        static void Init(memory::Allocator& allocator);

        static void Shutdown();

        static void ResizeWindow(ui32 width, ui32 height);
    };
}

#endif /* NP_ENGINE_RENDERER_IMPL_HPP */
