//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/21/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NSIT_LOG_HPP
#define NP_ENGINE_NSIT_LOG_HPP

#include <memory>
#include <string>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/FileSystem/FileSystem.hpp"

#include "NP-Engine/Vendor/SpdlogInclude.hpp"

namespace np::nsit
{
	class log
	{
	private:
		static atm_bl _initialized;
		static ::std::shared_ptr<::spdlog::logger> _file_logger;
		static ::std::shared_ptr<::spdlog::logger> _stdout_logger;
		static ::std::shared_ptr<::spdlog::logger> _logger;
		static ::std::shared_ptr<::spdlog::sinks::sink> _stdout_sink;
		static ::std::shared_ptr<::spdlog::sinks::sink> _file_sink;

	public:
		static inline void init()
		{
			bl expected = false;
			if (_initialized.compare_exchange_strong(expected, true, mo_release, mo_relaxed))
			{
				::std::string pattern = "%^[%l, %n, %Y-%m-%d %H:%M:%S.%e, pid:%P, tid:%t]%$\n%v\n";

				_stdout_sink = ::std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
				_stdout_sink->set_pattern(pattern);

				::std::string log_filepath = get_file_logger_file_path();
				_file_sink = ::std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_filepath, true);
				_file_sink->set_pattern(pattern);

				_logger = ::std::make_shared<spdlog::logger>("NP_ENGINE_LOG");
				_logger->sinks().push_back(_stdout_sink);
				_logger->sinks().push_back(_file_sink);
				spdlog::register_logger(_logger);
				_logger->set_level(spdlog::level::trace);
				_logger->flush_on(spdlog::level::trace);

				_file_logger = ::std::make_shared<spdlog::logger>("NP_ENGINE_FILE", _file_sink);
				spdlog::register_logger(_file_logger);
				_file_logger->set_level(spdlog::level::trace);
				_file_logger->flush_on(spdlog::level::trace);

				_stdout_logger = ::std::make_shared<spdlog::logger>("NP_ENGINE_STDOUT", _stdout_sink);
				spdlog::register_logger(_stdout_logger);
				_stdout_logger->set_level(spdlog::level::trace);
				_stdout_logger->flush_on(spdlog::level::trace);
			}
		}

		static inline ::std::string get_file_logger_file_path()
		{
			return fsys::append(fsys::get_current_path(), "NP-Engine-Log.log");
		}

		static inline ::std::shared_ptr<spdlog::logger>& get_file_logger()
		{
			init();
			return _file_logger;
		}

		static inline ::std::shared_ptr<spdlog::logger>& get_stdout_logger()
		{
			init();
			return _stdout_logger;
		}

		static inline ::std::shared_ptr<spdlog::logger>& get_logger()
		{
			init();
			return _logger;
		}
	};
} // namespace np::nsit

#endif /* NP_ENGINE_NSIT_LOG_HPP */
