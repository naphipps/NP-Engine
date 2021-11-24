//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/22/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Application/WindowLayer.hpp"

#include "NP-Engine/Graphics/GraphicsEvents.hpp"

namespace np::app
{
    window::Window* WindowLayer::CreateWindow(window::Window::Properties& properties)
    {
        memory::Block block = _windows.get_allocator().Allocate(sizeof(window::Window));
        memory::Construct<window::Window>(block, properties, _event_submitter);
        window::Window* window = _windows.emplace_back((window::Window*)block.Begin());
        _event_submitter.Emplace<graphics::GraphicsCreateRendererForWindowEvent>(*window);
        return window;
    }
}