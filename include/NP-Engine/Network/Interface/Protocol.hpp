//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/7/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NETWORK_INTERFACE_PROTOCOL_HPP
#define NP_ENGINE_NETWORK_INTERFACE_PROTOCOL_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::net
{
    enum class Protocol : ui32
    {
        None = 0,
        Udp,
        Tcp,
        Http,

        Max
    };
}

#endif /* NP_ENGINE_NETWORK_INTERFACE_PROTOCOL_HPP */