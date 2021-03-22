//
//  primitive_types.hpp
//  Project Space
//
//  Created by Nathan Phipps on 12/30/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_PRIMITIVE_TYPES_HPP
#define NP_ENGINE_PRIMITIVE_TYPES_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <cfloat>

using ui8 = uint8_t;
using ui16 = uint16_t;
using ui32 = uint32_t;
using ui64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using siz = size_t;

using flt = float;
using dbl = double;

using chr = char;
using uchr = unsigned char;

using bl = bool;

using str = ::std::string;

static str to_str(const chr* c)
{
    return str(c);
}

template <typename T>
static str to_str(T t)
{
    return ::std::to_string(t);
}

#endif /* NP_ENGINE_PRIMITIVE_TYPES_HPP */
