//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/18/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VENDOR_PCGCPP_INCLUDE_HPP
#define NP_ENGINE_VENDOR_PCGCPP_INCLUDE_HPP

#include "NP-Engine/Foundation/Foundation.hpp"

#include <pcg_random.hpp>

// force the usage of pcg_extras operators for pcg128 use cases with streams
using ::pcg_extras::operator<<;
using ::pcg_extras::operator>>;

#endif /* NP_ENGINE_VENDOR_PCGCPP_INCLUDE_HPP */