//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/19/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NSIT_INSIGHT_HPP
#define NP_ENGINE_NSIT_INSIGHT_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "Log.hpp"
#include "Instrumentor.hpp"
#include "ScopedTimer.hpp"
#include "Timer.hpp"
#include "InstrumentorTimer.hpp"
#include "TraceEvent.hpp"

#ifndef NP_ENGINE_PROFILE_ENABLE
	#define NP_ENGINE_PROFILE_ENABLE false
#endif

#if NP_ENGINE_PROFILE_ENABLE
	#define NP_ENGINE_PROFILE_SCOPE(name) ::np::nsit::instrumentor_timer NP_ENGINE_CONCATENATE(timer, __LINE__)(name)
	#define NP_ENGINE_PROFILE_FUNCTION() NP_ENGINE_PROFILE_SCOPE(NP_ENGINE_FUNCTION)
	#define NP_ENGINE_PROFILE_SAVE() ::np::nsit::instrumentor::save()
	#define NP_ENGINE_PROFILE_RESET() ::np::nsit::instrumentor::reset()
#else
	#define NP_ENGINE_PROFILE_SCOPE(name)
	#define NP_ENGINE_PROFILE_FUNCTION()
	#define NP_ENGINE_PROFILE_SAVE()
	#define NP_ENGINE_PROFILE_RESET()
#endif

#ifndef NP_ENGINE_LOG_ENABLE
	#define NP_ENGINE_LOG_ENABLE false
#endif

#if NP_ENGINE_LOG_ENABLE
	#define NP_ENGINE_LOG_TRACE(...) ::np::nsit::log::get_logger()->trace(__VA_ARGS__)
	#define NP_ENGINE_LOG_INFO(...) ::np::nsit::log::get_logger()->info(__VA_ARGS__)
	#define NP_ENGINE_LOG_WARN(...) ::np::nsit::log::get_logger()->warn(__VA_ARGS__)
	#define NP_ENGINE_LOG_ERROR(...) ::np::nsit::log::get_logger()->error(__VA_ARGS__)
	#define NP_ENGINE_LOG_CRITICAL(...) ::np::nsit::log::get_logger()->critical(__VA_ARGS__)
	#define NP_ENGINE_LOG_FILE_TRACE(...) ::np::nsit::log::get_file_logger()->trace(__VA_ARGS__)
	#define NP_ENGINE_LOG_FILE_INFO(...) ::np::nsit::log::get_file_logger()->info(__VA_ARGS__)
	#define NP_ENGINE_LOG_FILE_WARN(...) ::np::nsit::log::get_file_logger()->warn(__VA_ARGS__)
	#define NP_ENGINE_LOG_FILE_ERROR(...) ::np::nsit::log::get_file_logger()->error(__VA_ARGS__)
	#define NP_ENGINE_LOG_FILE_CRITICAL(...) ::np::nsit::log::get_file_logger()->critical(__VA_ARGS__)
	#define NP_ENGINE_LOG_STDOUT_TRACE(...) ::np::nsit::log::get_stdout_logger()->trace(__VA_ARGS__)
	#define NP_ENGINE_LOG_STDOUT_INFO(...) ::np::nsit::log::get_stdout_logger()->info(__VA_ARGS__)
	#define NP_ENGINE_LOG_STDOUT_WARN(...) ::np::nsit::log::get_stdout_logger()->warn(__VA_ARGS__)
	#define NP_ENGINE_LOG_STDOUT_ERROR(...) ::np::nsit::log::get_stdout_logger()->error(__VA_ARGS__)
	#define NP_ENGINE_LOG_STDOUT_CRITICAL(...) ::np::nsit::log::get_stdout_logger()->critical(__VA_ARGS__)
#else
	#define NP_ENGINE_LOG_TRACE(...)
	#define NP_ENGINE_LOG_INFO(...)
	#define NP_ENGINE_LOG_WARN(...)
	#define NP_ENGINE_LOG_ERROR(...)
	#define NP_ENGINE_LOG_CRITICAL(...)
	#define NP_ENGINE_LOG_FILE_TRACE(...)
	#define NP_ENGINE_LOG_FILE_INFO(...)
	#define NP_ENGINE_LOG_FILE_WARN(...)
	#define NP_ENGINE_LOG_FILE_ERROR(...)
	#define NP_ENGINE_LOG_FILE_CRITICAL(...)
	#define NP_ENGINE_LOG_STDOUT_TRACE(...)
	#define NP_ENGINE_LOG_STDOUT_INFO(...)
	#define NP_ENGINE_LOG_STDOUT_WARN(...)
	#define NP_ENGINE_LOG_STDOUT_ERROR(...)
	#define NP_ENGINE_LOG_STDOUT_CRITICAL(...)
#endif

#endif /* NP_ENGINE_NSIT_INSIGHT_HPP */
