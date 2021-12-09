//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/10/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_HPP
#define NP_ENGINE_WINDOW_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Event/Event.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Time/Time.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Concurrency/Concurrency.hpp"

#include "NP-Engine/Vendor/GlfwInclude.hpp"

// TODO: update comments

namespace np::window
{
	class Window : public event::EventHandler
	{
	public:
		constexpr static ui32 DEFAULT_WIDTH = 800;
		constexpr static ui32 DEFAULT_HEIGHT = 600;

		struct Properties
		{
			str Title = "NP Window";
			ui32 Width = DEFAULT_WIDTH;
			ui32 Height = DEFAULT_HEIGHT;
		};

	protected:
		Properties _properties;
		GLFWwindow* _glfw_window;
		concurrency::Thread _thread;
		atm_bl _show_procedure_is_complete;

		virtual void HandleClose(event::Event& event);

		virtual void HandleEvent(event::Event& event) override
		{
			switch (event.GetType())
			{
			case event::EVENT_TYPE_WINDOW_CLOSE:
				HandleClose(event);
				break;
			}
		}

		virtual void ShowProcedure();

		GLFWwindow* CreateGlfwWindow() const
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			return glfwCreateWindow(_properties.Width, _properties.Height, _properties.Title.c_str(), nullptr, nullptr);
		}

	public:
		Window(const Window::Properties& properties, event::EventSubmitter& event_submitter):
			event::EventHandler(event_submitter),
			_properties(properties),
			_glfw_window(CreateGlfwWindow()),
			_show_procedure_is_complete(true)
		{
			Show();
		}

		virtual ~Window()
		{
			glfwDestroyWindow(_glfw_window);
		}

		virtual void Show()
		{
			_show_procedure_is_complete.store(false, mo_release);
			_thread.Run(&Window::ShowProcedure, this);
		}

		virtual bl IsRunning() const
		{
			return !_show_procedure_is_complete.load(mo_acquire);
		}

		virtual ui32 GetWidth() const
		{
			return _properties.Width;
		}

		virtual ui32 GetHeight() const
		{
			return _properties.Height;
		}

		virtual str GetTitle() const
		{
			return _properties.Title;
		}

		virtual Properties GetProperties() const
		{
			return _properties;
		}

		virtual void Update(time::DurationMilliseconds duration_milliseconds)
		{
			// TODO: implement
		}

		virtual void SetTitle(str title)
		{
			_properties.Title = title;
			if (IsRunning() && _glfw_window != nullptr)
			{
				glfwSetWindowTitle(_glfw_window, _properties.Title.c_str());
			}
		}

		// virtual void AttachToRenderer() = 0; //TODO: we need to attach/detach from our renderer

		virtual void SetEnableVSync(bl enabled = true) {}

		virtual bl IsVSync() const
		{
			return false;
		}

		virtual void* GetNativeWindow() const
		{
			return _glfw_window;
		}

		virtual bl IsMinimized() const
		{
			return false;
		}

		virtual event::EventCategory GetHandledCategories() const override
		{
			return event::EVENT_CATEGORY_WINDOW;
		}
	};
} // namespace np::window

#endif /* NP_ENGINE_WINDOW_HPP */