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
		extern ::std::string initial_working_directory;
	} // namespace __detail

	using terminate_handler = ::std::terminate_handler;
	using signal_handler = void (*)(i32 signal);

	static inline ::std::string get_env(::std::string variable)
	{
		return ::std::getenv(variable.c_str());
	}

	static inline ::std::string get_default_working_directory()
	{
#if NP_ENGINE_PLATFORM_IS_APPLE
		return fsys::append(get_env("HOME"), "Library", "NP-Engine");

#elif NP_ENGINE_PLATFORM_IS_LINUX
		return fsys::append(get_env("HOME"), ".NP-Engine");

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
		return fsys::append(get_env("ALLUSERSPROFILE"), "NP-Engine");
#endif
	}

	static inline ::std::string get_initial_working_directory()
	{
		return __detail::initial_working_directory;
	}

	static inline void init()
	{
		nsit::log::init(); // log needs to be primed

		if (__detail::initial_working_directory.empty())
			__detail::initial_working_directory = fsys::get_current_path();

		::std::string working_directory = get_default_working_directory();
		fsys::create_directories(working_directory);
		fsys::set_current_path(working_directory);
	}

	static inline void set_terminate_handler(terminate_handler handler)
	{
		::std::set_terminate(handler);
	}

	static inline void set_signal_handler(signal_handler handler)
	{
		::std::signal(SIGABRT, handler);
		::std::signal(SIGFPE, handler);
		::std::signal(SIGILL, handler);
		::std::signal(SIGINT, handler);
		::std::signal(SIGSEGV, handler);
		::std::signal(SIGTERM, handler);
	}

	static inline i32 run(::std::string command)
	{
		return ::std::system(command.c_str());
	}
} // namespace np::sys

#endif /* NP_ENGINE_SYSTEM_HPP */
