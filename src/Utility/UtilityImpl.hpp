//
//  UtilityImpl.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/17/21.
//

#ifndef NP_ENGINE_UTILITY_IMPL_HPP
#define NP_ENGINE_UTILITY_IMPL_HPP

#include <utility>

#include "../TypeTraits/TypeTraits.hpp"

namespace np
{
    namespace utility
    {
        //TODO: use these everywhere
        
        /**
         forward
         */
        template <typename T>
        constexpr T&& Forward(typename typetraits::RemoveRefType<T>& arg) noexcept //TODO: should we add the remove referrence??
        {
            return ::std::forward<T>(arg);
        }
        
        template <typename T>
        constexpr T&& Forward(typename typetraits::RemoveRefType<T>&& arg) noexcept
        {
            return ::std::forward<T>(arg);
        }
    }
}

#endif /* NP_ENGINE_UTILITY_IMPL_HPP */
