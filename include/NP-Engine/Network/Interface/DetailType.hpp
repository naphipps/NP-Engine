//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/7/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NETWORK_INTERFACE_DETAIL_TYPE_HPP
#define NP_ENGINE_NETWORK_INTERFACE_DETAIL_TYPE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::net
{
    enum class DetailType : ui32
    {
        None = 0,
        Native,

        Max
    };
}

#endif /* NP_ENGINE_NETWORK_INTERFACE_DETAIL_TYPE_HPP */