//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/7/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_FRAMEBUFFER_HPP
#define NP_ENGINE_GPU_INTERFACE_FRAMEBUFFER_HPP

#include "NP-Engine/Memory/Memory.hpp"

#include "Detail.hpp"
#include "RenderPass.hpp"
#include "ImageResourceView.hpp"

namespace np::gpu
{
	struct Framebuffer : public DetailObject
	{
		static mem::sptr<Framebuffer> Create(mem::sptr<RenderPass> render_pass, siz width, siz height, siz layer_count,
											 const con::vector<mem::sptr<ImageResourceView>>& views);

		//TODO: ^ should we make all collection parameters be const refs? I think so

		virtual ~Framebuffer() = default;

		virtual mem::sptr<RenderPass> GetRenderPass() const = 0;

		virtual siz GetWidth() const = 0;

		virtual siz GetHeight() const = 0;

		virtual siz GetLayerCount() const = 0;

		virtual con::vector<mem::sptr<ImageResourceView>> GetImageResourceViews() const = 0;
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_FRAMEBUFFER_HPP */