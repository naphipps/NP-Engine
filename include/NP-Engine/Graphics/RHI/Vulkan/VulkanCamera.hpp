//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/18/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_CAMERA_HPP
#define NP_ENGINE_VULKAN_CAMERA_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/Graphics/RPI/RPI.hpp"

namespace np::gfx::rhi
{
	class VulkanCamera : public Camera
	{
	public:
		VulkanCamera& operator=(const Camera& other)
		{
			Fovy = other.Fovy;
			AspectRatio = other.AspectRatio;
			NearPlane = other.NearPlane;
			FarPlane = other.FarPlane;
			Eye = other.Eye;
			Center = other.Center;
			Up = other.Up;
			Update();

			return *this;
		}
	};
} // namespace np::gfx::rhi

#endif /* NP_ENGINE_VULKAN_CAMERA_HPP */