//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_RPI_RENDERER_HPP
#define NP_ENGINE_GRAPHICS_RPI_RENDERER_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Platform/Platform.hpp"

namespace np::graphics
{
	class Renderer
	{
	public:

		enum class RhiType
		{
			None,
			OpenGL,
			Vulkan,
			Direct3D,
			Metal
		};

	private:
		static RhiType _renderer_rhi_type; //TODO: should we refactor this to be "render rhi type" //TODO: set this as atomic??

	public:

		static inline RhiType GetRegisteredRhiType()
		{
			return _renderer_rhi_type;
		}

		static inline Renderer* Create(memory::Allocator& allocator);

		virtual void RegisterRhiType() const
		{
			_renderer_rhi_type = GetRhiType();
		}

		virtual RhiType GetRhiType() const = 0;

		virtual str GetName() const = 0;
		//TODO: virtual str GetVersion() const = 0;

		virtual bl IsEnabled() const = 0;
	};





	/*



	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1
		};
	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(const glm::vec4 & color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>&vertexArray, uint32_t indexCount = 0) = 0;

		static API GetAPI() { return s_API; }
		static Scope<RendererAPI> Create();
	private:
		static API s_API;
	};

}


*/




}

#endif /* NP_ENGINE_GRAPHICS_RPI_RENDERER_HPP */