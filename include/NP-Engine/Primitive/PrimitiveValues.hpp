//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/7/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_PRIMITIVE_VALUES_HPP
#define NP_ENGINE_PRIMITIVE_VALUES_HPP

#include "PrimitiveTypes.hpp"

#define BIT(n) ((::np::siz)1 << (::np::siz)n)
#define KILOBYTE_SIZE ((::np::siz)1000)
#define MEGABYTE_SIZE ((::np::siz)1000000)
#define GIGABYTE_SIZE ((::np::siz)1000000000)
#define TERABYTE_SIZE ((::np::siz)1000000000000)

#define KIBIBYTE_SIZE ((::np::siz)1024)
#define MEBIBYTE_SIZE ((::np::siz)1048576)
#define GIBIBYTE_SIZE ((::np::siz)1073741824)
#define TEBIBYTE_SIZE ((::np::siz)1099511627776)

#endif /* NP_ENGINE_PRIMITIVE_VALUES_HPP */
