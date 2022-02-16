//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/16/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_SCENE_HPP
#define NP_ENGINE_VULKAN_SCENE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "../../RPI/Scene.hpp"

namespace np::graphics::rhi
{
	class VulkanScene : public Scene
	{
	private:


	public:

		VulkanScene(Renderer& renderer) : Scene(renderer)
		{

		}

		virtual void Draw() override
		{
			_renderer->Draw();

			// TODO: telling the renderer to draw may require the usage of Command objects...

			// TODO: loop through Entities to get camera, and objects to renderer, etc
		}

		virtual void AdjustForWindowResize(window::Window& window) override
		{

		}
	};
}

#endif /* NP_ENGINE_VULKAN_SCENE_HPP */