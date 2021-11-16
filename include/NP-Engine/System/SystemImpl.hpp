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
            case SIGABRT:
                signal_string = "SIGABRT";
                break;
#ifdef SIGBREAK
            case SIGBREAK:
                signal_string = "SIGBREAK";
                break;
#endif
#ifdef SIGHUP
            case SIGHUP:
                signal_string = "SIGHUP";
                break;
#endif
#ifdef SIGQUIT
            case SIGQUIT:
                signal_string = "SIGQUIT";
                break;
#endif
#ifdef SIGTRAP
            case SIGTRAP:
                signal_string = "SIGTRAP";
                break;
#endif
#ifdef SIGPOLL
            case SIGPOLL:
                signal_string = "SIGPOLL";
                break;
#endif
/* SIGIOT is SIGINT for compadibility
#ifdef SIGIOT
            case SIGIOT:
                signal_string = "SIGIOT";
                break;
#endif
 */
#ifdef SIGEMT
            case SIGEMT:
                signal_string = "SIGEMT";
                break;
#endif
#ifdef SIGKILL
            case SIGKILL:
                signal_string = "SIGKILL";
                break;
#endif
#ifdef SIGBUS
            case SIGBUS:
                signal_string = "SIGBUS";
                break;
#endif
#ifdef SIGSYS
            case SIGSYS:
                signal_string = "SIGSYS";
                break;
#endif
#ifdef SIGPIPE
            case SIGPIPE:
                signal_string = "SIGPIPE";
                break;
#endif
#ifdef SIGALRM
            case SIGALRM:
                signal_string = "SIGALRM";
                break;
#endif
#ifdef SIGURG
            case SIGURG:
                signal_string = "SIGURG";
                break;
#endif
#ifdef SIGSTOP
            case SIGSTOP:
                signal_string = "SIGSTOP";
                break;
#endif
#ifdef SIGTSTP
            case SIGTSTP:
                signal_string = "SIGTSTP";
                break;
#endif
#ifdef SIGCONT
            case SIGCONT:
                signal_string = "SIGCONT";
                break;
#endif
#ifdef SIGCHLD
            case SIGCHLD:
                signal_string = "SIGCHLD";
                break;
#endif
#ifdef SIGTTIN
            case SIGTTIN:
                signal_string = "SIGTTIN";
                break;
#endif
#ifdef SIGTTOU
            case SIGTTOU:
                signal_string = "SIGTTOU";
                break;
#endif
#ifdef SIGIO
            case SIGIO:
                signal_string = "SIGIO";
                break;
#endif
#ifdef SIGXCPU
            case SIGXCPU:
                signal_string = "SIGXCPU";
                break;
#endif
#ifdef SIGXFSZ
            case SIGXFSZ:
                signal_string = "SIGXFSZ";
                break;
#endif
#ifdef SIGVTALRM
            case SIGVTALRM:
                signal_string = "SIGVTALRM";
                break;
#endif
#ifdef SIGPROF
            case SIGPROF:
                signal_string = "SIGPROF";
                break;
#endif
#ifdef SIGWINCH
            case SIGWINCH:
                signal_string = "SIGWINCH";
                break;
#endif
#ifdef SIGINFO
            case SIGINFO:
                signal_string = "SIGINFO";
                break;
#endif
#ifdef SIGUSR1
            case SIGUSR1:
                signal_string = "SIGUSR1";
                break;
#endif
#ifdef SIGUSR2
            case SIGUSR2:
                signal_string = "SIGUSR2";
                break;
#endif
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