//
//  Instrumentor.cpp
//  Project Space
//
//  Created by Nathan Phipps on 5/19/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#include "NP-Engine/Insight/Instrumentor.hpp"

namespace np
{
    namespace insight
    {
        const str Instrumentor::OtherData = "otherData";
        const str Instrumentor::TraceEvents = "traceEvents";
        Instrumentor Instrumentor::_instance;
    }
}
