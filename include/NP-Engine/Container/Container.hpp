//
//  Container.hpp
//  Project Space
//
//  Created by Nathan Phipps on 12/30/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_CONTAINER_HPP
#define NP_ENGINE_CONTAINER_HPP

//TODO: make custom containers that use our allocators

#include "NP-Engine/Foundation/Foundation.hpp"
#include "array.hpp"
#include "vector.hpp"
#include "queue.hpp"
#include "fixed_queue.hpp"
#include "uset.hpp"
#include "oset.hpp"
#include "deque.hpp"
#include "init_list.hpp"
#include "iterator.hpp"
//#include "deque_iterator.hpp"
#include "container_traits.hpp"

#include "NP-Engine/Memory/Memory.hpp"

#include <utility> //pair

#include <set>
#include <unordered_set>

#include <map>
#include <unordered_map>

namespace np::container
{
	template <class Key, class Compare = ::std::less<Key>>
	using omset = ::std::multiset<Key, Compare, memory::StdAllocator<Key>>;

	template <class Key, class T, class Compare = ::std::less<Key>>
	using ommap = ::std::multimap<Key, T, Compare, memory::StdAllocator<::std::pair<const Key, T>>>;


	template <class Key, class Hash = ::std::hash<Key>, class KeyEqualTo = ::std::equal_to<Key>>
	using umset = ::std::unordered_multiset<Key, Hash, KeyEqualTo, memory::StdAllocator<Key>>;

	template <class Key, class T, class Hash = ::std::hash<Key>, class KeyEqualTo = ::std::equal_to<Key>>
	using ummap = ::std::unordered_multimap<Key, T, Hash, KeyEqualTo, memory::StdAllocator<::std::pair<const Key, T>>>;
}


#endif /* NP_ENGINE_CONTAINER_HPP */
