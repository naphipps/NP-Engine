//
//  json.hpp
//  Project Space
//
//  Created by Nathan Phipps on 8/18/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_JSON_HPP
#define NP_ENGINE_JSON_HPP

#include <nlohmann/json.hpp>

namespace np::serialization::vendor::nlohmann
{
    //TODO: investigate boost json stuff -- apparenty it's much faster?
    //TODO: investigate rapidjson -- boost json is apparently on a newer version and may not be viable -- jason turner has great youtube video on this
    //TODO: a faster json vender would allow for faster profiles -- right now they're okay if they're isolated to a problem but can quickly slow down
    
    using namespace ::nlohmann;
}

#define NP_JSON_SPACING 4

#endif /* NP_ENGINE_JSON_HPP */
