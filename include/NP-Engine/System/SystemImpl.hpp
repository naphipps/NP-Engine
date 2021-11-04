//TODO: missing header

#ifndef NP_ENGINE_SYSTEM_IMPL_HPP
#define NP_ENGINE_SYSTEM_IMPL_HPP

#include <cstdlib>

#include "NP-Engine/Filesystem/Filesystem.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

//TODO: add summary comments

namespace np::system
{
	/**
	 gets the value of the given env variable
	*/
	static inline str GetEnv(str variable)
	{
		return ::std::getenv(variable.c_str());
	}


	static inline str GetDefaultWorkingDir()
	{
#if NP_ENGINE_PLATFORM_IS_APPLE
		//TODO: implement

#elif NP_ENGINE_PLATFORM_IS_LINUX
		//TODO: implement

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
		return fs::append(GetEnv("ALLUSERSPROFILE"), "NP-Engine");
#endif
	}


	static inline void Init()
	{
		str working_dir = GetDefaultWorkingDir();
		fs::create_directories(working_dir);
		fs::set_current_path(working_dir);
	}
}

#endif /* NP_ENGINE_SYSTEM_IMPL_HPP */