//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/19/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_INSIGHT_HPP
#define NP_ENGINE_INSIGHT_HPP

// TODO: I'm pretty sure Insight can now make use of Memory?

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
	#define NP_ENGINE_PROFILE_SCOPE(name) ::np::insight::InstrumentorTimer timer##__LINE__(name)
	#define NP_ENGINE_PROFILE_FUNCTION() NP_ENGINE_PROFILE_SCOPE(NP_FUNCTION)
	#define NP_ENGINE_PROFILE_SAVE() ::np::insight::Instrumentor::Save()
	#define NP_ENGINE_PROFILE_RESET() ::np::insight::Instrumentor::Reset()
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
	#define NP_ENGINE_LOG_TRACE(...) ::np::insight::Log::GetLogger()->trace(__VA_ARGS__)
	#define NP_ENGINE_LOG_INFO(...) ::np::insight::Log::GetLogger()->info(__VA_ARGS__)
	#define NP_ENGINE_LOG_WARN(...) ::np::insight::Log::GetLogger()->warn(__VA_ARGS__)
	#define NP_ENGINE_LOG_ERROR(...) ::np::insight::Log::GetLogger()->error(__VA_ARGS__)
	#define NP_ENGINE_LOG_CRITICAL(...) ::np::insight::Log::GetLogger()->critical(__VA_ARGS__)
	#define NP_ENGINE_LOG_FILE_TRACE(...) ::np::insight::Log::GetFileLogger()->trace(__VA_ARGS__)
	#define NP_ENGINE_LOG_FILE_INFO(...) ::np::insight::Log::GetFileLogger()->info(__VA_ARGS__)
	#define NP_ENGINE_LOG_FILE_WARN(...) ::np::insight::Log::GetFileLogger()->warn(__VA_ARGS__)
	#define NP_ENGINE_LOG_FILE_ERROR(...) ::np::insight::Log::GetFileLogger()->error(__VA_ARGS__)
	#define NP_ENGINE_LOG_FILE_CRITICAL(...) ::np::insight::Log::GetFileLogger()->critical(__VA_ARGS__)
	#define NP_ENGINE_LOG_STDOUT_TRACE(...) ::np::insight::Log::GetStdoutLogger()->trace(__VA_ARGS__)
	#define NP_ENGINE_LOG_STDOUT_INFO(...) ::np::insight::Log::GetStdoutLogger()->info(__VA_ARGS__)
	#define NP_ENGINE_LOG_STDOUT_WARN(...) ::np::insight::Log::GetStdoutLogger()->warn(__VA_ARGS__)
	#define NP_ENGINE_LOG_STDOUT_ERROR(...) ::np::insight::Log::GetStdoutLogger()->error(__VA_ARGS__)
	#define NP_ENGINE_LOG_STDOUT_CRITICAL(...) ::np::insight::Log::GetStdoutLogger()->critical(__VA_ARGS__)
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

#endif /* NP_ENGINE_INSIGHT_HPP */
