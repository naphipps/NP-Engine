//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/21/20
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Insight/Log.hpp"

namespace np::nsit
{
	atm_bl log::_initialized(false);
	::std::shared_ptr<spdlog::logger> log::_file_logger;
	::std::shared_ptr<spdlog::logger> log::_stdout_logger;
	::std::shared_ptr<spdlog::logger> log::_logger;
	::std::shared_ptr<::spdlog::sinks::sink> log::_stdout_sink;
	::std::shared_ptr<::spdlog::sinks::sink> log::_file_sink;
} // namespace np::nsit
