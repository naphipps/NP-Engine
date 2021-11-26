//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/22/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICES_CREATE_RENDERER_FOR_WINDOW_EVENT
#define NP_ENGINE_GRAPHICES_CREATE_RENDERER_FOR_WINDOW_EVENT

#include "NP-Engine/Event/Event.hpp"
#include "NP-Engine/Window/Window.hpp"

namespace np::graphics
{
	class GraphicsCreateRendererForWindowEvent : public event::Event
	{
	public:
		struct DataType
		{
			window::Window* window;
		};

		GraphicsCreateRendererForWindowEvent(window::Window& window): event::Event()
		{
			AssignData<DataType>({&window});
		}

		event::EventType GetType() const override
		{
			return event::EVENT_TYPE_GRAPHICS_CREATE_RENDERER_FOR_WINDOW;
		}

		event::EventCategory GetCategory() const override
		{
			return event::EVENT_CATEGORY_GRAPHICS;
		}
	};
} // namespace np::graphics

#endif /* NP_ENGINE_GRAPHICES_CREATE_RENDERER_FOR_WINDOW_EVENT */