//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/6/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NETWORK_HPP
#define NP_ENGINE_NETWORK_HPP

#include "NP-Engine/Foundation/Foundation.hpp"

#include "Interface/Interface.hpp"

namespace np::net
{
    void Init(DetailType detail_type);

    void Terminate(DetailType detail_type);

    void Update(DetailType detail_type);
}

#endif /* NP_ENGINE_NETWORK_HPP */