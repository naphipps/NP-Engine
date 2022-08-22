//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/21/20
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Insight/Log.hpp"

namespace np::nsit
{
	atm_bl Log::_initialized(false);
	::std::shared_ptr<spdlog::logger> Log::_file_logger;
	::std::shared_ptr<spdlog::logger> Log::_stdout_logger;
	::std::shared_ptr<spdlog::logger> Log::_logger;
	::std::shared_ptr<::spdlog::sinks::sink> Log::_stdout_sink;
	::std::shared_ptr<::spdlog::sinks::sink> Log::_file_sink;
} // namespace np::nsit
