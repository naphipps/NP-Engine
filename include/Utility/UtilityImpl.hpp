//
//  UtilityImpl.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/17/21.
//

#ifndef NP_ENGINE_UTILITY_IMPL_HPP
#define NP_ENGINE_UTILITY_IMPL_HPP

#include <utility>
#include <limits>

#include "TypeTraits/TypeTraits.hpp"

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
        
        /**
         move
         */
        template <class T>
        constexpr typetraits::RemoveRefType<T>&& Move(T&& t) noexcept
        {
            return ::std::move<T>(t);
        }
        
        /**
         swap
         */
        template <class T>
        constexpr void Swap(T& a, T& b) noexcept
        {
            ::std::swap<T>(a, b);
        }
        
        template <class T, siz N>
        constexpr void Swap(T (&a)[N], T (&b)[N]) noexcept
        {
            ::std::swap<T, N>(a, b);
        }
        
        /**
         limitis
         */
        template <class T>
        using NumericLimits = ::std::numeric_limits<T>;
    }
}

#endif /* NP_ENGINE_UTILITY_IMPL_HPP */
