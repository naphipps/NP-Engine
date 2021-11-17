//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/12/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_HPP
#define NP_ENGINE_GRAPHICS_HPP

#include "NP-Engine/Foundation/Foundation.hpp"

//TODO: move all contents of this file to Graphics.hpp, and Graphics.cpp

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/System/System.hpp"
#include "NP-Engine/String/String.hpp"

#include "RPI/Renderer.hpp"

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>//TODO: move this to math

//TODO: add summary comments

namespace np::graphics
{
	namespace __detail
	{
		template <class T>
		static inline Renderer* TryRenderer(memory::Allocator& allocator)
		{
			Renderer* renderer = nullptr;

			memory::Block block = allocator.Allocate(sizeof(T));
			if (block.IsValid())
			{
				memory::Construct<T>(block);
				renderer = static_cast<Renderer*>(block.Begin());

				if (!renderer->IsEnabled())
				{
					renderer = nullptr;
					memory::Destruct<T>(static_cast<T*>(renderer));
					allocator.Deallocate(block);
				}
			}
			else
			{
				allocator.Deallocate(block);
			}

			return renderer;
		}
	}

	void Init();

	void Shutdown();
}

#endif /* NP_ENGINE_GRAPHICS_HPP */
