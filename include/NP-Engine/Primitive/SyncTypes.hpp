//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/23/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_SYNC_TYPES_HPP
#define NP_ENGINE_SYNC_TYPES_HPP

#include <mutex>

using Mutex = ::std::mutex;
using Lock = ::std::lock_guard<::std::mutex>;

#endif /* NP_ENGINE_SYNC_TYPES_HPP */
