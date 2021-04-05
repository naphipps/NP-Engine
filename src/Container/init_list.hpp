//
//  init_list.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 3/24/21.
//

#ifndef NP_ENGINE_INIT_LIST_HPP
#define NP_ENGINE_INIT_LIST_HPP

#include <initializer_list>

namespace np::container
{
    /**
     brings std initializer list here
     */
    template <class T>
    using init_list = ::std::initializer_list<T>;
}

#endif /* NP_ENGINE_INIT_LIST_HPP */
