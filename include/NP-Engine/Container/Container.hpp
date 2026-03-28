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

#include "Entity.hpp"

// TODO: do we want to add to np::con?? BinarySearchTree, TwoFourTree, RedBlackTree, SoftHeap, etc

namespace np::con
{
	template <class T>
	using init_list = ::std::initializer_list<T>;

	template <class T, siz SIZE>
	using array = ::std::array<T, SIZE>;

	template <class T>
	using vector = ::std::vector<T, mem::std_allocator<T>>;

	/*
		//TODO: do we want to add array_list<T> ??
		template <class T>
		using array_list = vector<T>;
	*/

	template <class T>
	using deque = ::std::deque<T, mem::std_allocator<T>>;

	template <class T, class CONTAINER = con::deque<T>>
	using stack = ::std::stack<T, CONTAINER>;

	template <class T, class CONTAINER = con::deque<T>>
	using queue = ::std::queue<T, CONTAINER>;

	template <class T, class CONTAINER = con::vector<T>, class COMPARE = ::std::less<typename CONTAINER::value_type>>
	using pqueue = ::std::priority_queue<T, CONTAINER, COMPARE>;

	template <class T>
	using flist = ::std::forward_list<T, mem::std_allocator<T>>;

	template <class T>
	using list = ::std::list<T, mem::std_allocator<T>>;

	template <class KEY, class COMPARE = ::std::less<KEY>>
	using oset = ::std::set<KEY, COMPARE, mem::std_allocator<KEY>>;

	template <class KEY, class HASH = ::std::hash<KEY>, class KEY_EQUAL_TO = ::std::equal_to<KEY>>
	using uset = ::std::unordered_set<KEY, HASH, KEY_EQUAL_TO, mem::std_allocator<KEY>>;

	template <class KEY, class T, class COMPARE = ::std::less<KEY>>
	using omap = ::std::map<KEY, T, COMPARE, mem::std_allocator<::std::pair<const KEY, T>>>;

	template <class KEY, class T, class HASH = ::std::hash<KEY>, class KEY_EQUAL_TO = ::std::equal_to<KEY>>
	using umap = ::std::unordered_map<KEY, T, HASH, KEY_EQUAL_TO, mem::std_allocator<::std::pair<const KEY, T>>>;

	template <class KEY, class COMPARE = ::std::less<KEY>>
	using omset = ::std::multiset<KEY, COMPARE, mem::std_allocator<KEY>>;

	template <class KEY, class HASH = ::std::hash<KEY>, class KEY_EQUAL_TO = ::std::equal_to<KEY>>
	using umset = ::std::unordered_multiset<KEY, HASH, KEY_EQUAL_TO, mem::std_allocator<KEY>>;

	template <class KEY, class T, class COMPARE = ::std::less<KEY>>
	using ommap = ::std::multimap<KEY, T, COMPARE, mem::std_allocator<::std::pair<const KEY, T>>>;

	template <class KEY, class T, class HASH = ::std::hash<KEY>, class KEY_EQUAL_TO = ::std::equal_to<KEY>>
	using ummap = ::std::unordered_multimap<KEY, T, HASH, KEY_EQUAL_TO, mem::std_allocator<::std::pair<const KEY, T>>>;
} // namespace np::con

namespace np::mem
{
	static inline void copy_bytes(void* dst, const con::vector<ui8>& bytes)
	{
		copy_bytes(dst, bytes.data(), bytes.size());
	}
} //namespace np::mem

#endif /* NP_ENGINE_CONTAINER_HPP */