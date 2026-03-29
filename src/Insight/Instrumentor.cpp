//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/19/20
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Insight/Instrumentor.hpp"

namespace np::nsit
{
	mutexed_wrapper<instrumentor::properties> instrumentor::_properties;
} // namespace np::nsit
