//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/21/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_LOG_HPP
#define NP_ENGINE_LOG_HPP

#include <memory>
#include <string>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Filesystem/Filesystem.hpp"

#include "NP-Engine/Vendor/SpdlogInclude.hpp"

namespace np::insight
{
	class Log
	{
	private:
		static atm_bl _initialized;
		static ::std::shared_ptr<::spdlog::logger> _file_logger;
		static ::std::shared_ptr<::spdlog::logger> _stdout_logger;
		static ::std::shared_ptr<::spdlog::logger> _logger;
		static ::std::shared_ptr<::spdlog::sinks::sink> _stdout_sink;
		static ::std::shared_ptr<::spdlog::sinks::sink> _file_sink;

	public:
		static inline void Init()
		{
			if (!_initialized.load(mo_acquire))
			{
				_initialized.store(true, mo_release);

				::std::string pattern = "%^[pid:%P, tid:%t, %Y-%m-%d %H:%M:%S.%e] [%n, %l]%$ %v";

				_stdout_sink = ::std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
				_stdout_sink->set_pattern(pattern);

				::std::string log_filepath = GetFileLoggerFilePath();
				_file_sink = ::std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_filepath, true);
				_file_sink->set_pattern(pattern);

				_logger = ::std::make_shared<spdlog::logger>("NP_LOG");
				_logger->sinks().push_back(_stdout_sink);
				_logger->sinks().push_back(_file_sink);
				spdlog::register_logger(_logger);
				_logger->set_level(spdlog::level::trace);
				_logger->flush_on(spdlog::level::trace);

				_file_logger = ::std::make_shared<spdlog::logger>("NP_FILE", _file_sink);
				spdlog::register_logger(_file_logger);
				_file_logger->set_level(spdlog::level::trace);
				_file_logger->flush_on(spdlog::level::trace);

				_stdout_logger = ::std::make_shared<spdlog::logger>("NP_STDOUT", _stdout_sink);
				spdlog::register_logger(_stdout_logger);
				_stdout_logger->set_level(spdlog::level::trace);
				_stdout_logger->flush_on(spdlog::level::trace);
			}
		}

		static inline ::std::string GetFileLoggerFilePath()
		{
			return fs::Append(fs::GetCurrentPath(), "NP-Engine-Log.log");
		}

		static inline ::std::shared_ptr<spdlog::logger>& GetFileLogger()
		{
			Init();
			return _file_logger;
		}

		static inline ::std::shared_ptr<spdlog::logger>& GetStdoutLogger()
		{
			Init();
			return _stdout_logger;
		}

		static inline ::std::shared_ptr<spdlog::logger>& GetLogger()
		{
			Init();
			return _logger;
		}
	};
} // namespace np::insight

#ifndef NP_LOG_ENABLE
	#define NP_LOG_ENABLE 0
#endif

#if NP_LOG_ENABLE

	#define NP_LOG_TRACE(...) ::np::insight::Log::GetLogger()->trace(__VA_ARGS__)
	#define NP_LOG_INFO(...) ::np::insight::Log::GetLogger()->info(__VA_ARGS__)
	#define NP_LOG_WARN(...) ::np::insight::Log::GetLogger()->warn(__VA_ARGS__)
	#define NP_LOG_ERROR(...) ::np::insight::Log::GetLogger()->error(__VA_ARGS__)
	#define NP_LOG_CRITICAL(...) ::np::insight::Log::GetLogger()->critical(__VA_ARGS__)

	#define NP_LOG_FILE_TRACE(...) ::np::insight::Log::GetFileLogger()->trace(__VA_ARGS__)
	#define NP_LOG_FILE_INFO(...) ::np::insight::Log::GetFileLogger()->info(__VA_ARGS__)
	#define NP_LOG_FILE_WARN(...) ::np::insight::Log::GetFileLogger()->warn(__VA_ARGS__)
	#define NP_LOG_FILE_ERROR(...) ::np::insight::Log::GetFileLogger()->error(__VA_ARGS__)
	#define NP_LOG_FILE_CRITICAL(...) ::np::insight::Log::GetFileLogger()->critical(__VA_ARGS__)

	#define NP_LOG_STDOUT_TRACE(...) ::np::insight::Log::GetStdoutLogger()->trace(__VA_ARGS__)
	#define NP_LOG_STDOUT_INFO(...) ::np::insight::Log::GetStdoutLogger()->info(__VA_ARGS__)
	#define NP_LOG_STDOUT_WARN(...) ::np::insight::Log::GetStdoutLogger()->warn(__VA_ARGS__)
	#define NP_LOG_STDOUT_ERROR(...) ::np::insight::Log::GetStdoutLogger()->error(__VA_ARGS__)
	#define NP_LOG_STDOUT_CRITICAL(...) ::np::insight::Log::GetStdoutLogger()->critical(__VA_ARGS__)

#else

	#define NP_LOG_TRACE(...)
	#define NP_LOG_INFO(...)
	#define NP_LOG_WARN(...)
	#define NP_LOG_ERROR(...)
	#define NP_LOG_CRITICAL(...)

	#define NP_LOG_FILE_TRACE(...)
	#define NP_LOG_FILE_INFO(...)
	#define NP_LOG_FILE_WARN(...)
	#define NP_LOG_FILE_ERROR(...)
	#define NP_LOG_FILE_CRITICAL(...)

	#define NP_LOG_STDOUT_TRACE(...)
	#define NP_LOG_STDOUT_INFO(...)
	#define NP_LOG_STDOUT_WARN(...)
	#define NP_LOG_STDOUT_ERROR(...)
	#define NP_LOG_STDOUT_CRITICAL(...)

#endif // NP_LOG_ENABLE

#endif /* NP_ENGINE_LOG_HPP */
