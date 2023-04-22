//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/8/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_RENDER_SHADER_HPP
#define NP_ENGINE_VULKAN_RENDER_SHADER_HPP

#include <fstream>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/FileSystem/FileSystem.hpp"
#include "NP-Engine/System/System.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "../../Interface/Interface.hpp"

#include "VulkanRenderDevice.hpp"

namespace np::gfx::__detail
{
	class VulkanRenderShader : public RenderShader
	{
	private:
		//VulkanDevice& _device;
		siz _size;
		str _filename_spv;
		con::vector<ui32> _bytes;
		VkShaderModule _shader_module;

		static VkShaderModuleCreateInfo CreateShaderModuleInfo()
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
			cmd += " " + fsys::Append(".", _properties.filename);
			cmd += " -o " + fsys::Append(".", _filename_spv);

			if (fsys::Exists(_properties.filename))
			{
				NP_ENGINE_LOG_INFO("Compiling: '" + cmd + "'");
				sys::Run(cmd);
			}
			else
			{
				NP_ENGINE_LOG_ERROR("Cannot find: '" + _properties.filename + "'\n" + sys::GetDefaultWorkingDir());
			}
		}

		void Read(str filename)
		{
			_properties.filename = filename;
			_filename_spv = _properties.filename + ".spv";
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

		void CreateModule()
		{
			Dispose();

			if (Size() > 0)
			{
				VulkanRenderDevice& vulkan_device = (VulkanRenderDevice&)(*_device);
				VkShaderModuleCreateInfo shader_module_info = CreateShaderModuleInfo();
				shader_module_info.codeSize = Size();
				shader_module_info.pCode = (ui32*)Bytes();
				if (vkCreateShaderModule(vulkan_device, &shader_module_info, nullptr, &_shader_module) != VK_SUCCESS)
					_shader_module = nullptr;
			}
		}

		void Dispose()
		{
			if (_shader_module)
			{
				VulkanRenderDevice& vulkan_device = (VulkanRenderDevice&)(*_device);
				vkDestroyShaderModule(vulkan_device, _shader_module, nullptr);
				_shader_module = nullptr;
			}
		}

	public:
		VulkanRenderShader(mem::sptr<RenderDevice> device, Properties& properties):
			RenderShader(device, properties),
			_size(0),
			_shader_module(nullptr)
		{
			Load(_properties.filename);
		}

		~VulkanRenderShader()
		{
			Dispose();
		}

		mem::sptr<RenderDevice> GetRenderDevice() const
		{
			return _device;
		}

		void Load(str filename) override
		{
			Read(filename);
			CreateModule();
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
} // namespace np::gfx::__detail

#endif /* NP_ENGINE_VULKAN_RENDER_SHADER_HPP */
