//
//  TypeTraitsImpl.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/17/21.
//

#ifndef NP_ENGINE_TYPE_TRAITS_IMPL_HPP
#define NP_ENGINE_TYPE_TRAITS_IMPL_HPP

#include <type_traits>

#include "NP-Engine/Primitive/Primitive.hpp"

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
        
        /**
         enable if
         */
        template <bl B, class T = void>
        using EnableIf = ::std::enable_if<B, T>;
        
        template <bl B, class T = void>
        using EnableIfType = ::std::enable_if_t<B, T>;
        
        /**
         is constructible
         */
        template <class T, class ... Args>
        inline constexpr bl IsConstructible = ::std::is_constructible_v<T, Args...>;
        
        template <class T, class ... Args>
        using IsConstructibleObject = ::std::is_constructible<T, Args...>;
        
        template <class T>
        inline constexpr bl IsDefaultConstructible = ::std::is_default_constructible_v<T>;
        
        template <class T>
        using IsDefaultConstructibleObject = ::std::is_default_constructible<T>; //TODO: refactor these "Object" names to "Type"
        
        template <class T>
        inline constexpr bl IsCopyConstructible = ::std::is_copy_constructible_v<T>;
        
        template <class T>
        using IsCopyConstructibleObject = ::std::is_copy_constructible<T>;
        
        template <class T>
        inline constexpr bl IsMoveConstructible = ::std::is_move_constructible_v<T>;
        
        template <class T>
        using IsMoveConstructibleObject = ::std::is_move_constructible<T>;
        
        template <class T>
        inline constexpr bl IsCopyAssignable = ::std::is_copy_assignable_v<T>;
        
        template <class T>
        using IsCopyAssignableObject = ::std::is_copy_assignable<T>;
        
        template <class T>
        inline constexpr bl IsMoveAssignable = ::std::is_move_assignable_v<T>;
        
        template <class T>
        using IsMoveAssignableObject = ::std::is_move_assignable<T>;
        
        template <class T>
        inline constexpr bl IsDestructible = ::std::is_destructible_v<T>;
        
        template <class T>
        using IsDestructibleObject = ::std::is_destructible<T>;
    }
}

#define NP_IF_IS_SAME(T1, T2) if constexpr (::np::typetraits::IsSame<T1, T2>)
#define NP_ELIF_IS_SAME(T1, T2) else NP_IF_IS_SAME(T1, T2)

#define NP_IF_IS_BASE_OF(T1, T2) if constexpr (::np::typetraits::IsBaseOf<T1, T2>)
#define NP_ELIF_IS_BASE_OF(T1, T2) else NP_IF_IS_BASE_OF(T1, T2)

#endif /* NP_ENGINE_TYPE_TRAITS_IMPL_HPP */
