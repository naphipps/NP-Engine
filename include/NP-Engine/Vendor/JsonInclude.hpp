//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/18/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VENDOR_JSON_INCLUDE_HPP
#define NP_ENGINE_VENDOR_JSON_INCLUDE_HPP

#include "NP-Engine/Foundation/Foundation.hpp"

#include <nlohmann/json.hpp>

#define NP_ENGINE_JSON_SPACING 4

// TODO: investigate boost json stuff -- apparenty it's much faster?
// TODO: investigate rapidjson -- boost json is apparently on a newer version and may not be viable -- jason turner has great
// youtube video on this
// TODO: a faster json vender would allow for faster profiles -- right now they're okay if they're isolated to a problem but can
// quickly slow down
// TODO: I feel like this needs to be it's own folder that is independet of other folders and allows us to define our macros
// like we like

#endif /* NP_ENGINE_VENDOR_JSON_INCLUDE_HPP */