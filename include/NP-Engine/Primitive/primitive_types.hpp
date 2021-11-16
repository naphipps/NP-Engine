//
//  primitive_types.hpp
//  Project Space
//
//  Created by Nathan Phipps on 12/30/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

//TODO: refactor these "primitive_types" files to Pascal casing

#ifndef NP_ENGINE_PRIMITIVE_TYPES_HPP
#define NP_ENGINE_PRIMITIVE_TYPES_HPP

#include <cstddef>
#include <cstdint>
#include <cfloat>

using nptr = ::std::nullptr_t;

using ui8 = uint8_t;
using ui16 = uint16_t;
using ui32 = uint32_t;
using ui64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using siz = size_t;
using dif = ptrdiff_t;

using flt = float;
using dbl = double;

using chr = char;
using uchr = unsigned char;
using wchr = wchar_t;
using u16chr = char16_t;
using u32chr = char32_t;

using bl = bool;

#endif /* NP_ENGINE_PRIMITIVE_TYPES_HPP */
