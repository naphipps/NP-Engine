//
//  TypeTraitsImpl.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/17/21.
//

#ifndef NP_ENGINE_TYPE_TRAITS_IMPL_HPP
#define NP_ENGINE_TYPE_TRAITS_IMPL_HPP

#include <type_traits>

#include "Primitive/Primitive.hpp"

namespace np
{
    namespace typetraits
    {
        /**
         remove ref
         */
        template <typename T>
        using RemoveRef = ::std::remove_reference<T>;
        
        template <typename T>
        using RemoveRefType = ::std::remove_reference_t<T>;
        
        /**
         is same
         */
        template <typename T, typename U>
        using IsSameObject = ::std::is_same<T, U>;
        
        template <typename T, typename U>
        inline constexpr bl IsSame = ::std::is_same_v<T, U>;
        
        /**
         is base of
         */
        template <typename Base, typename Derived>
        using IsBaseOfObject = ::std::is_base_of<Base, Derived>;
        
        template <typename Base, typename Derived>
        inline constexpr bl IsBaseOf = ::std::is_base_of_v<Base, Derived>;
        
        /**
         is pointer
         */
        template <class T>
        using IsPointerObject = ::std::is_pointer<T>;
        
        template <class T>
        inline constexpr bl IsPointer = ::std::is_pointer_v<T>;
        
        /**
         remove pointer
         */
        template <class T>
        using RemovePointer = ::std::remove_pointer<T>;
        
        template <class T>
        using RemovePointerObject = ::std::remove_pointer_t<T>;
    }
}

#define NP_IF_IS_SAME(T1, T2) if constexpr (::np::typetraits::IsSame<T1, T2>)
#define NP_ELIF_IS_SAME(T1, T2) else NP_IF_IS_SAME(T1, T2)

#define NP_IF_IS_BASE_OF(T1, T2) if constexpr (::np::typetraits::IsBaseOf<T1, T2>)
#define NP_ELIF_IS_BASE_OF(T1, T2) else NP_IF_IS_BASE_OF(T1, T2)

#endif /* NP_ENGINE_TYPE_TRAITS_IMPL_HPP */
