//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/8/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_SHADER_HPP
#define NP_ENGINE_VULKAN_SHADER_HPP

#include <fstream>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Math/Math.hpp"
#include "NP-Engine/Filesystem/Filesystem.hpp"
#include "NP-Engine/System/System.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "../../RPI/RPI.hpp"

#include "VulkanDevice.hpp"

namespace np::graphics::rhi
{
	class VulkanShader : public Shader
	{
	private:
		VulkanDevice& _device;
		siz _size;
		str _filename_spv;
		container::vector<ui32> _bytes;
		VkShaderModule _shader_module;

		VkShaderModuleCreateInfo CreateShaderModuleInfo()
		{
			VkShaderModuleCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			return info;
		}

		void CompileSpirV()
		{
			// TODO: do we want to do a hash check to see if we need to compile or not?

			str cmd(NP_ENGINE_VULKAN_GLSLC);
			cmd += " -fshader-stage=" + GetShaderStage();
			cmd += " " + fs::Append(".", _filename);
			cmd += " -o " + fs::Append(".", _filename_spv);

			if (fs::Exists(_filename))
			{
				::std::cout << "Compiling: '" + cmd + "'\n";
				system::Run(cmd);
			}
			else
			{
				::std::cout << "cannot find: '" << _filename << "'\n";
				::std::cout << system::GetDefaultWorkingDir() << "\n";
			}
		}

		void Read(str filename)
		{
			_filename = filename;
			_filename_spv = _filename + ".spv";
			CompileSpirV();
			::std::ifstream ifile(_filename_spv, ::std::ios::ate | ::std::ios::binary);

			if (ifile.is_open())
			{
				_size = (siz)ifile.tellg();
				_bytes.resize((siz)::std::ceill((ldbl)_size / (ldbl)sizeof(ui32)));
				ifile.seekg(0);
				ifile.read((chr*)_bytes.data(), _size);
				ifile.close();
			}
			else
			{
				_size = 0;
				_bytes.clear();
			}
		}

		void LoadModule()
		{
			DestroyModule();

			if (Size() > 0)
			{
				VkShaderModuleCreateInfo shader_module_info = CreateShaderModuleInfo();
				shader_module_info.codeSize = Size();
				shader_module_info.pCode = (ui32*)Bytes();
				if (vkCreateShaderModule(_device, &shader_module_info, nullptr, &_shader_module) != VK_SUCCESS)
				{
					_shader_module = nullptr;
				}
			}
		}

		void DestroyModule()
		{
			if (_shader_module != nullptr)
			{
				vkDestroyShaderModule(_device, _shader_module, nullptr);
				_shader_module = nullptr;
			}
		}

	public:
		VulkanShader(VulkanDevice& device, str filename, Type type, str entrypoint = "main"):
			Shader(filename, type, entrypoint),
			_device(device),
			_size(0),
			_shader_module(nullptr)
		{
			Load(_filename);
		}

		~VulkanShader()
		{
			DestroyModule();
		}

		VulkanInstance& GetInstance() const
		{
			return _device.GetInstance();
		}

		VulkanSurface& GetSurface() const
		{
			return _device.GetSurface();
		}

		VulkanDevice& GetDevice() const
		{
			return _device;
		}

		void Load(str filename) override
		{
			Read(filename);
			LoadModule();
		}

		siz Size() const override
		{
			return _size;
		}

		void* Bytes() const override
		{
			return _bytes.empty() ? nullptr : (void*)_bytes.data();
		}

		operator VkShaderModule() const
		{
			return _shader_module;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_SHADER_HPP */
