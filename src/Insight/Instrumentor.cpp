//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/19/20
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Insight/Instrumentor.hpp"

namespace np::nsit
{
	mutex Instrumentor::_m;
	atm_bl Instrumentor::_initialized;
	::std::shared_ptr<::rapidjson::Document> Instrumentor::_report;
	::std::string Instrumentor::_filepath;
	atm_bl Instrumentor::_save_on_trace;
	atm_bl Instrumentor::_enable_trace;
} // namespace np::nsit
