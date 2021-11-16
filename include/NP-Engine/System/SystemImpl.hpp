//TODO: missing header

#ifndef NP_ENGINE_SYSTEM_IMPL_HPP
#define NP_ENGINE_SYSTEM_IMPL_HPP

#include <cstdlib>
#include <iostream>
#include <csignal>
#include <exception>

#include "NP-Engine/Filesystem/Filesystem.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "Popup.hpp"

//TODO: add summary comments

namespace np::system
{
    namespace __detail
    {
        static inline void terminate_handler() noexcept
        {
            ::std::cerr << "The terminate function was called.\nLog file can be found here : " + insight::Log::GetFileLoggerFilePath() << "\n";
            Popup::Show("NP-Engine Terminate Function Called",
                "Probably an unhandled exception was thrown.\nLog file can be found here : " + insight::Log::GetFileLoggerFilePath(),
                Popup::Style::Error, Popup::Buttons::OK);
        }

        static inline void signal_handler(int signal) noexcept
        {
            ::std::string signal_string = "";
            switch (signal)
            {
            case SIGINT:
                signal_string = "SIGINT";
                break;
            case SIGILL:
                signal_string = "SIGILL";
                break;
            case SIGFPE:
                signal_string = "SIGFPE";
                break;
            case SIGSEGV:
                signal_string = "SIGSEGV";
                break;
            case SIGTERM:
                signal_string = "SIGTERM";
                break;
            case SIGBREAK:
                signal_string = "SIGBREAK";
                break;
            case SIGABRT:
                signal_string = "SIGABRT";
                break;
            default:
                signal_string = "UNKNOWN SIGNAL";
                break;
            }

            ::std::cerr << signal_string + " was raised.\nLog file can be found here : " + insight::Log::GetFileLoggerFilePath() << "\n";
            Popup::Show("NP-Engine Signal Raised",
                signal_string + " was raised.\nLog file can be found here : " + insight::Log::GetFileLoggerFilePath(),
                Popup::Style::Error, Popup::Buttons::OK);
        }
    }

	/**
	 gets the value of the given env variable
	*/
	static inline ::std::string GetEnv(::std::string variable)
	{
		return ::std::getenv(variable.c_str());
	}


	static inline ::std::string GetDefaultWorkingDir()
	{
#if NP_ENGINE_PLATFORM_IS_APPLE
        return fs::append(GetEnv("HOME"), fs::append("Library", "NP-Engine"));

#elif NP_ENGINE_PLATFORM_IS_LINUX
		return fs::append(GetEnv("HOME"), ".NP-Engine");

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
		return fs::append(GetEnv("ALLUSERSPROFILE"), "NP-Engine");
#endif
	}


	static inline void Init()
	{
        ::std::string working_dir = GetDefaultWorkingDir();
		fs::create_directories(working_dir);
		fs::set_current_path(working_dir);

        ::std::set_terminate(__detail::terminate_handler);
        ::std::signal(SIGABRT, __detail::signal_handler);
        ::std::signal(SIGFPE, __detail::signal_handler);
        ::std::signal(SIGILL, __detail::signal_handler);
        ::std::signal(SIGINT, __detail::signal_handler);
        ::std::signal(SIGSEGV, __detail::signal_handler);
        ::std::signal(SIGTERM, __detail::signal_handler);
	}
}

#endif /* NP_ENGINE_SYSTEM_IMPL_HPP */