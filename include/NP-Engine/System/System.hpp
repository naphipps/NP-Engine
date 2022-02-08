//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/4/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_SYSTEM_HPP
#define NP_ENGINE_SYSTEM_HPP

#include "NP-Engine/Foundation/Foundation.hpp"

#include <cstdlib>
#include <iostream>
#include <csignal>
#include <exception>

#include "NP-Engine/Filesystem/Filesystem.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"

namespace np::system
{
	using TerminateHandler = ::std::terminate_handler;
	typedef void (*SignalHandler)(i32 signal);

	static inline ::std::string GetEnv(::std::string variable)
	{
		return ::std::getenv(variable.c_str());
	}

	static inline ::std::string GetDefaultWorkingDir()
	{
#if NP_ENGINE_PLATFORM_IS_APPLE
		return fs::Append(fs::Append(GetEnv("HOME"), "Library"), "NP-Engine");

#elif NP_ENGINE_PLATFORM_IS_LINUX
		return fs::Append(GetEnv("HOME"), ".NP-Engine");

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
		return fs::Append(GetEnv("ALLUSERSPROFILE"), "NP-Engine");
#endif
	}

	static inline void Init()
	{
		::std::string working_dir = GetDefaultWorkingDir();
		fs::CreateDirectories(working_dir);
		fs::SetCurrentPath(working_dir);
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
} // namespace np::system

#endif /* NP_ENGINE_SYSTEM_HPP */
