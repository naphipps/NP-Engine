//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/30/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_CONTAINER_HPP
#define NP_ENGINE_CONTAINER_HPP

#include "NP-Engine/Foundation/Foundation.hpp"

#include "NP-Engine/Memory/Memory.hpp"

#include <utility> //pair

#include <array>
#include <deque>
#include <queue>
#include <stack>
#include <vector>
#include <list>
#include <forward_list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <initializer_list>

namespace np::container
{
	template <class T>
	using init_list = ::std::initializer_list<T>;
	
    template <class T, siz SIZE>
    using array = ::std::array<T, SIZE>;
    
    template <class T>
    using vector = ::std::vector<T, memory::StdAllocator<T>>;
    
    template <class T>
    using deque = ::std::deque<T, memory::StdAllocator<T>>;
    
    template <class T, class Container = container::deque<T>>
    using stack = ::std::stack<T, Container>;
    
    template <class T, class Container = container::deque<T>>
    using queue = ::std::queue<T, Container>;
    
    template <class T, class Container = container::vector<T>, class Compare = ::std::less<typename Container::value_type>>
    using pqueue = ::std::priority_queue<T, Container, Compare>;
    
    template <class T>
    using flist = ::std::forward_list<T, memory::StdAllocator<T>>;
    
    template <class T>
    using list = ::std::list<T, memory::StdAllocator<T>>;
    
    template <class Key, class Compare = ::std::less<Key>>
    using oset = ::std::set<Key, Compare, memory::StdAllocator<Key>>;

    template <class Key, class Hash = ::std::hash<Key>, class KeyEqualTo = ::std::equal_to<Key>> 
    using uset = ::std::unordered_set<Key, Hash, KeyEqualTo, memory::StdAllocator<Key>>;
	
    template<class Key, class T, class Compare = ::std::less<Key>>
    using omap = ::std::map<Key, T, Compare, memory::StdAllocator<::std::pair<const Key, T>>>;
    
    template<class Key, class T, class Hash = ::std::hash<Key>, class KeyEqualTo = ::std::equal_to<Key>>
    using umap = ::std::unordered_map<Key, T, Hash, KeyEqualTo, memory::StdAllocator<::std::pair<const Key, T>>>;
    
	template <class Key, class Compare = ::std::less<Key>>
	using omset = ::std::multiset<Key, Compare, memory::StdAllocator<Key>>;
	
	template <class Key, class Hash = ::std::hash<Key>, class KeyEqualTo = ::std::equal_to<Key>>
	using umset = ::std::unordered_multiset<Key, Hash, KeyEqualTo, memory::StdAllocator<Key>>;

	template <class Key, class T, class Compare = ::std::less<Key>>
	using ommap = ::std::multimap<Key, T, Compare, memory::StdAllocator<::std::pair<const Key, T>>>;
	
	template <class Key, class T, class Hash = ::std::hash<Key>, class KeyEqualTo = ::std::equal_to<Key>>
	using ummap = ::std::unordered_multimap<Key, T, Hash, KeyEqualTo, memory::StdAllocator<::std::pair<const Key, T>>>;
}

#endif /* NP_ENGINE_CONTAINER_HPP */