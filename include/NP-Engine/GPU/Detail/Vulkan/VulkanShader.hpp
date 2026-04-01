//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/8/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_SHADER_HPP
#define NP_ENGINE_GPU_VULKAN_SHADER_HPP

#include <fstream>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/FileSystem/FileSystem.hpp"
#include "NP-Engine/System/System.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanDevice.hpp"
#include "VulkanStage.hpp"

namespace np::gpu::__detail
{
	class VulkanShader : public Shader
	{
	private:
		mem::sptr<VulkanDevice> _device;
		VulkanStage _stage;
		str _filename;
		str _entrypoint;
		str _filename_spv;
		siz _size; // we keep track of byte count in _size separate from _bytes.size() since it may not be ui32 aligned
		con::vector<ui32> _bytes; //using ui32 since VkShaderModuleCreateInfo requires a ui32*
		VkShaderModule _module;

		static VkShaderModuleCreateInfo CreateVkInfo()
		{
			VkShaderModuleCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			return info;
		}

		void CompileSpirV()
		{
			// TODO: do we want to do a hash check to see if we need to compile or not?

			str cmd(NP_ENGINE_VULKAN_GLSLC);
			cmd += " -fshader-stage=" + _stage.GetCompileName();
			cmd += " " + fsys::append(".", _filename);
			cmd += " -o " + fsys::append(".", _filename_spv);

			if (fsys::exists(_filename))
			{
				NP_ENGINE_LOG_INFO("Compiling: '" + cmd + "'");
				sys::run(cmd);
			}
			else
			{
				NP_ENGINE_LOG_ERROR("Cannot find: '" + _filename + "'\n" + sys::get_default_working_directory());
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

		void CreateVkModule()
		{
			Dispose();

			if (_size > 0)
			{
				VkShaderModuleCreateInfo info = CreateVkInfo();
				info.codeSize = _size;
				info.pCode = info.codeSize > 0 ? _bytes.data() : nullptr;

				mem::sptr<VulkanInstance> instance = _device->GetDetailInstance();
				VkResult result = vkCreateShaderModule(*_device->GetLogicalDevice(), &info, instance->GetVulkanAllocationCallbacks(), &_module);
				if (result != VK_SUCCESS)
					_module = nullptr;
			}
		}

		void Dispose()
		{
			if (_module)
			{
				mem::sptr<VulkanInstance> instance = _device->GetDetailInstance();
				vkDestroyShaderModule(*_device->GetLogicalDevice(), _module, instance->GetVulkanAllocationCallbacks());
				_module = nullptr;
			}
		}

	public:
		VulkanShader(mem::sptr<Device> device, Stage stage, str filename, str entrypoint):
			_device(DetailObject::EnsureIsDetailType(device, DetailType::Vulkan)),
			_stage(stage),
			_filename(filename),
			_entrypoint(entrypoint),
			_filename_spv(""),
			_size(0),
			_bytes({}),
			_module(nullptr)
		{
			Load(_filename);
		}

		virtual ~VulkanShader()
		{
			Dispose();
		}

		operator VkShaderModule() const
		{
			return _module;
		}

		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _device->GetServices();
		}

		virtual mem::sptr<Device> GetDevice() const override
		{
			return _device;
		}

		virtual void Load(str filename) override
		{
			Read(filename);
			CreateVkModule();
		}

		virtual void Reload() override
		{
			Load(_filename);
		}

		virtual str GetFilename() const override
		{
			return _filename;
		}

		virtual str GetEntrypoint() const override
		{
			return _entrypoint;
		}

		virtual void SetEntrypoint(str entrypoint) override
		{
			_entrypoint = entrypoint;
		}

		virtual Stage GetStage() const override
		{
			return _stage;
		}

		VkPipelineShaderStageCreateInfo GetVkPipelineShaderStageCreateInfo() const
		{
			VkPipelineShaderStageCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			info.stage = _stage.GetVkShaderStageFlagBits();
			info.module = _module;
			info.pName = _entrypoint.c_str();
			//info.pSpecializationInfo; //TODO: add support for specialization info (aka: "value overrides")
			return info;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_RENDER_SHADER_HPP */
