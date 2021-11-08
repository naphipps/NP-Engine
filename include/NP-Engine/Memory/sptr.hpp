//
//  sptr.hpp
//  Project Space
//
//  Created by Nathan Phipps on 2/7/21.
//  Copyright © 2021 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_SPTR_HPP
#define NP_ENGINE_SPTR_HPP

#include <memory>
#include <utility>

namespace np
{
    namespace memory
    {
        template <typename T>
        using sptr = ::std::shared_ptr<T>;
        
        template <typename T, typename ... Args>
        constexpr sptr<T> CreateSptr(Args&& ... args)
        {
            return ::std::make_shared<T>(::std::forward<Args>(args)...);
        }
    }
}

#endif /* NP_ENGINE_SPTR_HPP */
