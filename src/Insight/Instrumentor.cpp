//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/19/20
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Insight/Instrumentor.hpp"

namespace np::insight
{
	const ::std::string Instrumentor::OtherData = "otherData";
	const ::std::string Instrumentor::TraceEvents = "traceEvents";
	Instrumentor Instrumentor::_instance;
} // namespace np::insight
