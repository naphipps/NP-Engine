//
//  uptr.hpp
//  Project Space
//
//  Created by Nathan Phipps on 2/7/21.
//  Copyright © 2021 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_UPTR_HPP
#define NP_ENGINE_UPTR_HPP

#include <memory>
#include <utility>

namespace np
{
    namespace memory
    {
        template <typename T>
        using uptr = ::std::unique_ptr<T>;

        template <typename T, typename ... Args>
        constexpr uptr<T> CreateUptr(Args&& ... args)
        {
            return ::std::make_unique<T>(::std::forward<Args>(args)...);
        }
    }
}

#endif /* NP_ENGINE_UPTR_HPP */
