//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/30/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_CONTAINER_HPP
#define NP_ENGINE_CONTAINER_HPP

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

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "NP-Engine/Vendor/ConcurrentqueueInclude.hpp"

// TODO: do we want to add to np::con?? BinarySearchTree, TwoFourTree, RedBlackTree, SoftHeap, etc

namespace np::con
{
	template <class T>
	using init_list = ::std::initializer_list<T>;

	template <class T, siz SIZE>
	using array = ::std::array<T, SIZE>;

	template <class T>
	using vector = ::std::vector<T, mem::StdAllocator<T>>;

	template <class T>
	using deque = ::std::deque<T, mem::StdAllocator<T>>;

	template <class T, class Container = con::deque<T>>
	using stack = ::std::stack<T, Container>;

	template <class T, class Container = con::deque<T>>
	using queue = ::std::queue<T, Container>;

	template <class T, class Container = con::vector<T>, class Compare = ::std::less<typename Container::value_type>>
	using pqueue = ::std::priority_queue<T, Container, Compare>;

	template <class T>
	using flist = ::std::forward_list<T, mem::StdAllocator<T>>;

	template <class T>
	using list = ::std::list<T, mem::StdAllocator<T>>;

	template <class Key, class Compare = ::std::less<Key>>
	using oset = ::std::set<Key, Compare, mem::StdAllocator<Key>>;

	template <class Key, class Hash = ::std::hash<Key>, class KeyEqualTo = ::std::equal_to<Key>>
	using uset = ::std::unordered_set<Key, Hash, KeyEqualTo, mem::StdAllocator<Key>>;

	template <class Key, class T, class Compare = ::std::less<Key>>
	using omap = ::std::map<Key, T, Compare, mem::StdAllocator<::std::pair<const Key, T>>>;

	template <class Key, class T, class Hash = ::std::hash<Key>, class KeyEqualTo = ::std::equal_to<Key>>
	using umap = ::std::unordered_map<Key, T, Hash, KeyEqualTo, mem::StdAllocator<::std::pair<const Key, T>>>;

	template <class Key, class Compare = ::std::less<Key>>
	using omset = ::std::multiset<Key, Compare, mem::StdAllocator<Key>>;

	template <class Key, class Hash = ::std::hash<Key>, class KeyEqualTo = ::std::equal_to<Key>>
	using umset = ::std::unordered_multiset<Key, Hash, KeyEqualTo, mem::StdAllocator<Key>>;

	template <class Key, class T, class Compare = ::std::less<Key>>
	using ommap = ::std::multimap<Key, T, Compare, mem::StdAllocator<::std::pair<const Key, T>>>;

	template <class Key, class T, class Hash = ::std::hash<Key>, class KeyEqualTo = ::std::equal_to<Key>>
	using ummap = ::std::unordered_multimap<Key, T, Hash, KeyEqualTo, mem::StdAllocator<::std::pair<const Key, T>>>;
} // namespace np::con

#endif /* NP_ENGINE_CONTAINER_HPP */