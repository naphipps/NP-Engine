//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_SCENE_HPP
#define NP_ENGINE_RPI_SCENE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/ECS/ECS.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Platform/Platform.hpp"

#include "NP-Engine/Vendor/EnttInclude.hpp"

#include "RhiType.hpp"
#include "Renderer.hpp"

namespace np::graphics
{
	class Scene
	{
	protected:
		::entt::registry _registry;
		Renderer* _renderer;

	public:
		
		static Scene* Create(memory::Allocator& allocator, Renderer& renderer);
		
		Scene(Renderer& renderer): _renderer(memory::AddressOf(renderer)) {}

		// TODO: this should end needing a Create function like we do in Renderer, then having RHI implementation

		graphics::Renderer* GetRenderer() const
		{
			return _renderer;
		}

		virtual void Draw() = 0;
		virtual void AdjustForWindowResize(window::Window& window) = 0;
	};
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_SCENE_HPP */