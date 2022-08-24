//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/4/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_SYSTEM_HPP
#define NP_ENGINE_SYSTEM_HPP

#include <cstdlib>
#include <iostream>
#include <csignal>
#include <exception>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/FileSystem/FileSystem.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"

namespace np::sys
{
	namespace __detail
	{
		extern ::std::string InitialWorkingDir;
	}

	using TerminateHandler = ::std::terminate_handler;
	typedef void (*SignalHandler)(i32 signal);

	static inline ::std::string GetEnv(::std::string variable)
	{
		return ::std::getenv(variable.c_str());
	}

	static inline ::std::string GetDefaultWorkingDir()
	{
#if NP_ENGINE_PLATFORM_IS_APPLE
		return fsys::Append(fsys::Append(GetEnv("HOME"), "Library"), "NP-Engine");

#elif NP_ENGINE_PLATFORM_IS_LINUX
		return fsys::Append(GetEnv("HOME"), ".NP-Engine");

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
		return fsys::Append(GetEnv("ALLUSERSPROFILE"), "NP-Engine");
#endif
	}

	static inline ::std::string GetInitalWorkingDir()
	{
		return __detail::InitialWorkingDir;
	}

	static inline void Init()
	{
		if (__detail::InitialWorkingDir.empty())
			__detail::InitialWorkingDir = fsys::GetCurrentPath();

		::std::string working_dir = GetDefaultWorkingDir();
		fsys::CreateDirectories(working_dir);
		fsys::SetCurrentPath(working_dir);
	}

	static inline void SetTerminateHandler(TerminateHandler handler)
	{
		::std::set_terminate(handler);
	}

	static inline void SetSignalHandler(SignalHandler handler)
	{
		::std::signal(SIGABRT, handler);
		::std::signal(SIGFPE, handler);
		::std::signal(SIGILL, handler);
		::std::signal(SIGINT, handler);
		::std::signal(SIGSEGV, handler);
		::std::signal(SIGTERM, handler);
	}

	static inline i32 Run(::std::string command)
	{
		return ::std::system(command.c_str());
	}
} // namespace np::sys

#endif /* NP_ENGINE_SYSTEM_HPP */
