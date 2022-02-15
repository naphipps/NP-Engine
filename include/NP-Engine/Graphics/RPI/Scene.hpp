//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_SCENE_HPP
#define NP_ENGINE_RPI_SCENE_HPP

#include "NP-Engine/ECS/ECS.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "NP-Engine/Vendor/EnttInclude.hpp"

#include "Renderer.hpp"

namespace np::graphics
{
	// TODO: Scene needs to become a pure virtual class since implmentations are RHI dependent

	class Scene
	{
	private:
		::entt::registry _registry;

		Renderer* _renderer;

	public:
		Scene(Renderer& renderer): _renderer(memory::AddressOf(renderer)) {}

		Scene(): _renderer(nullptr) {}

		// TODO: this should end needing a Create function like we do in Renderer, then having RHI implementation

		void AttachRenderer(Renderer& renderer)
		{
			_renderer = memory::AddressOf(renderer);
		}

		bl HasRenderer() const
		{
			return _renderer != nullptr;
		}

		graphics::Renderer& Renderer()
		{
			NP_ASSERT(HasRenderer(), "Cannot get renderer when one is not attached to this scene.");
			return *_renderer;
		}

		const graphics::Renderer& Renderer() const
		{
			NP_ASSERT(HasRenderer(), "Cannot get renderer when one is not attached to this scene.");
			return *_renderer;
		}

		void Draw()
		{
			NP_ASSERT(HasRenderer(), "Cannot draw scene without renderer.");

			Renderer().Draw();

			// TODO: loop through Entities to get camera, and objects to renderer, etc
		}

		virtual void AdjustForWindowResize(window::Window& window) {} // TODO: make this pure virtual
	};
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_SCENE_HPP */