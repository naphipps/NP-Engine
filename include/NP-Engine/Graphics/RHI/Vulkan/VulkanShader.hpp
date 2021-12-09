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

#include "../../RPI/Shader.hpp"

namespace np::graphics::rhi
{
	class VulkanShader : public Shader
	{
	private:
		siz _size;
		str _filename_spv;
		container::vector<ui32> _bytes;

		void CompileSpirV()
		{
			str cmd(NP_ENGINE_VULKAN_GLSLC);
			cmd += " -fshader-stage=" + GetShaderStage();
			cmd += " " + fs::append(".", _filename);
			cmd += " -o " + fs::append(".", _filename_spv);

			if (fs::exists(_filename))
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

	public:
		VulkanShader(str filename, Type type): Shader(filename, type), _size(0)
		{
			Load(_filename);
		}

		void Load(str filename) override
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

		siz Size() const override
		{
			return _size;
		}

		void* Bytes() const override
		{
			return _bytes.empty() ? nullptr : (void*)_bytes.data();
		}

		Type GetType() const
		{
			return _type;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_SHADER_HPP */
