//
//  deque.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 3/23/21.
//

#ifndef NP_ENGINE_DEQUE_HPP
#define NP_ENGINE_DEQUE_HPP

#include <deque> //TODO: remove this
#include <algorithm> // lexicographical_compare //TODO: remove this?
#include <type_traits>
#include <limits>
#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "deque_iterator.hpp"
#include "iterator.hpp"
#include "init_list.hpp"
#include "vector.hpp"

namespace np
{
    namespace container
    {
        template <class T>
        using std_deque = ::std::deque<T>;
        
        //TODO: add summary comments
        
        template <class T>
        class deque
        {
        private:
            NP_STATIC_ASSERT(::std::is_move_constructible_v<T>, "T must be move constructible");
            NP_STATIC_ASSERT(::std::is_copy_constructible_v<T>, "T must be copy constructible");
            NP_STATIC_ASSERT(::std::is_default_constructible_v<T>, "T must be default constructible");
            NP_STATIC_ASSERT(::std::is_destructible_v<T>, "T must be destructible");
            
        public:
            using value_type = T;
            using allocator_type = memory::Allocator;
            using allocator_reference = allocator_type&;
            using const_allocator_reference = const allocator_type&;
            using allocator_pointer = allocator_type*;
            using size_type = siz;
            using difference_type = dif;
            
            using iterator = deque_iterator<T*>;
            using const_iterator = const deque_iterator<T*>;
            using reference = typename iterator::reference;
            using const_reference = typename iterator::const_reference;
            using pointer = typename iterator::pointer;
            using const_pointer = typename iterator::const_pointer;
            
            using reverse_iterator = deque_reverse_iterator<T*>;
            using const_reverse_iterator = const deque_reverse_iterator<T*>;
            
        private:
            using TRAITS = deque_traits<T>;
            
            vector<pointer> _buffers;
            siz _begin_index;
            siz _size;
            
            void destroy(iterator it)
            {
                memory::Destruct<T>(it.base());
            }
            
            void destroy(iterator begin, iterator end)
            {
                for (iterator it = begin; it != end; it++)
                {
                    destroy(it);
                }
            }
            
            void shift_elements_right(iterator old_begin, iterator old_end, iterator new_begin, iterator new_end)
            {
                NP_ASSERT((old_end - old_begin) == (new_end - new_begin),
                          "we need to make sure our iterators line up with an equal number of elements");
                
                memory::Block block{.size = TRAITS::T_SIZE};
                reverse_iterator old_rend(old_begin);
                reverse_iterator old_rit(old_end);
                reverse_iterator new_rit(new_end);
                
                while (old_rit != old_rend /* && new_rit != new_rend */)
                {
                    block.ptr = new_rit.base();
                    memory::Construct<T>(block, ::std::move(*old_rit));
                    memory::Destruct<T>(old_rit.base());
                    old_rit++;
                    new_rit++;
                }
            }
            
            void shift_elements_left(iterator old_begin, iterator old_end, iterator new_begin, iterator new_end)
            {
                NP_ASSERT((old_end - old_begin) == (new_end - new_begin),
                          "we need to make sure our iterators line up with an equal number of elements");
                
                memory::Block block{.size = TRAITS::T_SIZE};
                iterator old_it(old_begin);
                iterator new_it(new_begin);
                
                while (old_it != old_end /* && new_it != new_end */)
                {
                    block.ptr = new_it.base();
                    memory::Construct<T>(block, ::std::move(*old_it));
                    memory::Destruct<T>(old_it.base());
                    old_it++;
                    new_it++;
                }
            }
            
            void prepend_buffer()
            {
                _buffers.insert(_buffers.begin(), get_allocator().Allocate(TRAITS::BUFFER_BLOCK_SIZE).Begin());
            }
            
            void prepend_buffers(siz count)
            {
                for (siz i=0; i<count; i++)
                {
                    prepend_buffer();
                }
            }
            
            void append_buffer()
            {
                _buffers.emplace_back((pointer)get_allocator().Allocate(TRAITS::BUFFER_BLOCK_SIZE).Begin());
            }
            
            void append_buffers(siz count)
            {
                for (siz i=0; i<count; i++)
                {
                    append_buffer();
                }
            }
            
            void deallocate_front_buffer()
            {
                get_allocator().Deallocate(_buffers.front());
                _buffers.erase(_buffers.begin());
            }
            
            void deallocate_front_buffers(siz count)
            {
                for (siz i=0; i<count; i++)
                {
                    deallocate_front_buffer();
                }
            }
            
            void deallocate_back_buffer()
            {
                get_allocator().Deallocate(_buffers.back());
                _buffers.erase(_buffers.end() - 1);
            }
            
            void deallocate_back_buffers(siz count)
            {
                for (siz i=0; i<count; i++)
                {
                    deallocate_back_buffer();
                }
            }
            
            void append_capacity(siz count)
            {
                if (count > 0) //minor optimization
                {
                    siz back_capacity = _buffers.size() * TRAITS::BUFFER_SIZE - _begin_index + _size;
                    //we want one more buffer if we fill up our last one, hence <=
                    if (back_capacity <= count)
                    {
                        append_buffers((count - back_capacity) / TRAITS::BUFFER_SIZE + 1);
                    }
                }
            }
            
            void append(siz count, const_reference value)
            {
                if (count > 0) //minor optimization
                {
                    iterator it = end();
                    append_capacity(count);
                    _size += count;
                    memory::Block block{.size = TRAITS::T_SIZE};
                    iterator e = end();
                    for (; it != e; it++)
                    {
                        block.ptr = it.base();
                        memory::Construct<T>(block, value);
                    }
                }
            }
            
            void copy_from(const deque& other)
            {
                _begin_index = other._begin_index;
                _size = other._size;
                append_buffers(other._buffers.size() - _buffers.size());
                memory::Block block{.size = TRAITS::T_SIZE};
                iterator dst = begin();
                
                for (iterator src = other.begin(); src != other.end(); src++)
                {
                    block.ptr = dst.base();
                    memory::Construct<T>(block, *src);
                    dst++;
                }
            }
            
            void move_from(deque&& other)
            {
                _begin_index = ::std::move(other._begin_index);
                _size = ::std::move(other._size);
                append_buffers(other._buffers.size() - _buffers.size());
                memory::Block block{.size = TRAITS::T_SIZE};
                iterator dst = begin();
                
                for (iterator src = other.begin(); src != other.end(); src++)
                {
                    block.ptr = dst.base();
                    memory::Construct<T>(block, ::std::move(*src));
                    dst++;
                }
            }
            
            void deallocate_buffers()
            {
                allocator_reference allocator = get_allocator();
                for (siz i=0; i<_buffers.size(); i++)
                {
                    allocator.Deallocate(_buffers[i]);
                }
            }
            
            void init(allocator_reference allocator)
            {
                if (!_buffers.empty())
                {
                    destroy(begin(), end());
                    deallocate_buffers();
                    _buffers.clear();
                }
                
                _buffers.set_allocator(allocator);
                append_buffer();
                _begin_index = 0;
                _size = 0;
            }
            
            void init()
            {
                init(get_allocator());
            }
            
        public:
            
            deque()
            {
                init();
            }
            
            deque(allocator_reference allocator):
            _buffers(allocator)
            {
                init();
            }
            
            deque(allocator_reference allocator, const deque& other):
            deque(allocator)
            {
                copy_from(other);
            }
            
            deque(const deque& other):
            deque(other.get_allocator(), other)
            {}
            
            deque(allocator_reference allocator, deque&& other):
            deque(allocator)
            {
                move_from(other);
            }
            
            deque(deque&& other):
            deque(other.get_allocator(), other)
            {}
            
            deque(allocator_reference allocator, siz count, const_reference value):
            deque(allocator)
            {
                append(count, value);
            }
            
            deque(siz count, const_reference value):
            deque()
            {
                assign(count, value);
            }
            
            deque(allocator_reference allocator, siz count):
            deque(allocator, count, T())
            {}
            
            deque(siz count):
            deque(count, T())
            {}
            
            deque(allocator_reference allocator, init_list<T> list):
            _buffers(allocator)
            {
                assign(list);
            }
            
            deque(init_list<T> list):
            deque()
            {
                assign(list);
            }
            
            ~deque()
            {
                destroy(begin(), end());
                deallocate_buffers();
            }
            
            allocator_reference get_allocator()
            {
                return _buffers.get_allocator();
            }
            
            const_allocator_reference get_allocator() const
            {
                return _buffers.get_allocator();
            }
            
            bl set_allocator(allocator_reference allocator)
            {
                allocator_reference old_allocator = _buffers.get_allocator();
                bl set = _buffers.set_allocator(allocator);
                NP_ASSERT(set, "failed to set allocator on buffer vector in set_allocator");
                
                memory::Block buffer_block;
                memory::Block block{.size = TRAITS::T_SIZE};
                for (siz b=0; b<_buffers.size(); b++)
                {
                    //move buffer to new allocator
                    buffer_block = allocator.Allocate(TRAITS::BUFFER_BLOCK_SIZE);
                    set &= buffer_block.IsValid();
                    NP_ASSERT(set, "failed allocation for new buffer block in set_allocator");
                    
                    //move all the elements to new buffer
                    siz finish = b == _buffers.size() - 1 ? (_begin_index + _size) % TRAITS::BUFFER_SIZE : TRAITS::BUFFER_SIZE;
                    for (siz e = b == 0 ? _begin_index : 0; e < finish; e++)
                    {
                        //move element and destruct the old one
                        block.ptr = (pointer)buffer_block.ptr + e;
                        set &= memory::Construct<T>(block, ::std::move(_buffers[b][e]));
                        NP_ASSERT(set, "failed construction for new element in set_allocator");
                        set &= memory::Destruct<T>(memory::AddressOf(_buffers[b][e]));
                        NP_ASSERT(set, "failed destruction for old element in set_allocator");
                    }
                    
                    //deallocate old buffer and set to new buffer
                    set &= old_allocator.Deallocate(_buffers[b]);
                    _buffers[b] = buffer_block.ptr;
                }
                
                NP_ASSERT(set, "our set_allocator calls should always be successful");
                return set;
            }
            
            void assign(init_list<T> list)
            {
                init();
                
                if (TRAITS::BUFFER_SIZE <= list.size())
                {
                    append_buffers(list.size() / TRAITS::BUFFER_SIZE);
                }
                _size = list.size();
                
                memory::Block block{.size = TRAITS::T_SIZE};
                iterator b = begin();
                iterator e = end();
                siz i = 0;
                for (iterator it = b; it != e; it++)
                {
                    block.ptr = it.base();
                    memory::Construct<T>(block, list[i++]);
                }
            }
            
            void assign(siz count, const_reference value)
            {
                init();
                append(count, value);
            }
            
            void assign(const_iterator begin, const_iterator end)
            {
                NP_ASSERT(end > begin, "end must be greater than begin for iterator");
                init();
                
                siz size = end - begin;
                if (TRAITS::BUFFER_SIZE <= size)
                {
                    append_buffers(size / TRAITS::BUFFER_SIZE);
                }
                _size = size;
                
                memory::Block block{.size = TRAITS::T_SIZE};
                iterator dst = this->begin();
                for (iterator src = begin; src != end; src++)
                {
                    block.ptr = dst.base();
                    memory::Construct<T>(block, *src);
                    dst++;
                }
            }
            
            void assign(const_reverse_iterator rbegin, const_reverse_iterator rend)
            {
                NP_ASSERT(rbegin > rend, "rbegin must be greater than rend for reverse_iterator");
                init();
                
                siz size = rbegin - rend;
                if (TRAITS::BUFFER_SIZE <= size)
                {
                    append_buffers(size / TRAITS::BUFFER_SIZE);
                }
                _size = size;
                
                memory::Block block{.size = TRAITS::T_SIZE};
                reverse_iterator dst = this->rbegin();
                for (reverse_iterator src = rbegin; src != rend; src++)
                {
                    block.ptr = dst.base();
                    memory::Construct<T>(block, *src);
                    dst++;
                }
            }
            
            deque& operator=(const deque& other)
            {
                init();
                copy_from(other);
                return *this;
            }
            
            deque& operator=(deque&& other)
            {
                init();
                move_from(other);
                return *this;
            }
            
            deque& operator=(init_list<T> list)
            {
                assign(list);
                return *this;
            }
            
            
            reference at(siz index)
            {
                NP_ASSERT(0 >= index && index < size(), "given index was invalid");
                
                siz i = index + _begin_index;
                siz buffer_index = i / TRAITS::BUFFER_SIZE; //intentially drop remainder
                siz element_index = i - buffer_index * TRAITS::BUFFER_SIZE; //calc remainder -- chose to not use modulus since we already divide
                
                return _buffers[buffer_index][element_index];
            }
            
            const_reference at(siz index) const
            {
                NP_ASSERT(0 >= index && index < size(), "given index was invalid");
                
                siz i = index + _begin_index;
                siz buffer_index = i / TRAITS::BUFFER_SIZE; //intentially drop remainder
                siz element_index = i - buffer_index * TRAITS::BUFFER_SIZE; //calc remainder -- chose to not use modulus since we already divide
                
                return _buffers[buffer_index][element_index];
            }
            
            reference operator[](siz index)
            {
                return at(index);
            }
            
            const_reference operator[](siz index) const
            {
                return at(index);
            }
            
            reference front()
            {
                return at(0);
            }
            
            const_reference front() const
            {
                return at(0);
            }
            
            reference back()
            {
                return at(size() - 1);
            }
            
            const_reference back() const
            {
                return at(size() - 1);
            }
            
            siz size() const
            {
                return _size;
            }
            
            siz max_size() const
            {
                return ::std::numeric_limits<siz>::max();
            }
            
            bl empty() const
            {
                return size() == 0;
            }
            
            void shrink_to_fit()
            {
                //does nothing because we already keep a minimum memory footprint
            }
            
            void clear() noexcept
            {
                destroy(begin(), end());
                deallocate_buffers();
                _buffers.clear();
                init();
            }
            
            iterator begin()
            {
                return iterator(_buffers.begin(), _buffers.front() + _begin_index);
            }
            
            iterator end()
            {
                return iterator(_buffers.end() - 1, _buffers.back() + ((_begin_index + _size) % TRAITS::BUFFER_SIZE));
            }
            
            const_iterator begin() const
            {
                return const_iterator(_buffers.begin(), _buffers.front() + _begin_index);
            }
            
            const_iterator end() const
            {
                return const_iterator(_buffers.end() - 1, _buffers.back() + ((_begin_index + _size) % TRAITS::BUFFER_SIZE));
            }
            
            reverse_iterator rbegin()
            {
                return reverse_iterator(_buffers.end() - 1, _buffers.back() + ((_begin_index + _size) % TRAITS::BUFFER_SIZE));
            }
            
            reverse_iterator rend()
            {
                return reverse_iterator(_buffers.begin(), _buffers.front() + _begin_index);
            }
            
            const_reverse_iterator rbegin() const
            {
                return const_reverse_iterator(_buffers.end() - 1, _buffers.back() + ((_begin_index + _size) % TRAITS::BUFFER_SIZE));
            }
            
            const_reverse_iterator rend() const
            {
                return const_reverse_iterator(_buffers.begin(), _buffers.front() + _begin_index);
            }
            
            const_iterator cbegin() const
            {
                return begin();
            }
            
            const_iterator cend() const
            {
                return end();
            }
            
            const_reverse_iterator crbegin() const
            {
                return rbegin();
            }
            
            const_reverse_iterator crend() const
            {
                return rend();
            }
            
            bl contains(const_iterator it)
            {
                return begin() <= it && it < end();
            }
            
            bl contains(const_reverse_iterator it)
            {
                return rend() <= it && it < rbegin();
            }
            
            iterator insert(const_iterator dst, const_reference value)
            {
                iterator inserted;
                if (contains(dst))
                {
                    iterator old_end = end();
                    append_capacity(1);
                    _size++;
                    shift_elements_right(dst, old_end, dst + 1, end());
                    memory::Block block{.ptr = dst.base(), .size = TRAITS::T_SIZE};
                    memory::Construct<T>(block, value);
                    inserted = dst;
                }
                return inserted;
            }
            
            iterator insert(const_iterator dst, T&& value)
            {
                iterator inserted;
                if (contains(dst))
                {
                    iterator old_end = end();
                    append_capacity(1);
                    _size++;
                    shift_elements_right(dst, old_end, dst + 1, end());
                    memory::Block block{.ptr = dst.base(), .size = TRAITS::T_SIZE};
                    memory::Construct<T>(block, ::std::move(value));
                    inserted = dst;
                }
                return inserted;
            }
            
            iterator insert(const_iterator dst, siz count, const_reference value)
            {
                iterator inserted;
                if (contains(dst))
                {
                    iterator old_end = end();
                    append_capacity(count);
                    _size += count;
                    shift_elements_right(dst, old_end, dst + count, end());
                    memory::Block block{.size = TRAITS::T_SIZE};
                    for (siz i=0; i<count; i++)
                    {
                        block.ptr = (dst + i).base();
                        memory::Construct<T>(block, value);
                    }
                    inserted = dst;
                }
                return inserted;
            }
            
            iterator insert(const_iterator dst, container::iterator<T> first, container::iterator<T> last)
            {
                iterator inserted;
                if (contains(dst))
                {
                    iterator old_end = end();
                    siz count = last - first;
                    append_capacity(count);
                    _size += count;
                    shift_elements_right(dst, old_end, dst + (last - first), end());
                    memory::Block block{.size = TRAITS::T_SIZE};
                    for (container::iterator<T> it = first; it != last; it++)
                    {
                        block.ptr = (dst + (it - first)).base();
                        memory::Construct<T>(block, *it);
                    }
                    inserted = dst;
                }
                return inserted;
            }
            
            iterator insert(const_iterator dst, container::reverse_iterator<T> rfirst, container::reverse_iterator<T> rlast)
            {
                iterator inserted;
                if (contains(dst))
                {
                    iterator old_end = end();
                    siz count = rfirst - rlast;
                    append_capacity(count);
                    _size += count;
                    shift_elements_right(dst, old_end, dst + (rfirst - rlast), end());
                    memory::Block block{.size = TRAITS::T_SIZE};
                    for (container::reverse_iterator<T> rit = rfirst; rit != rlast; rit++)
                    {
                        block.ptr = (dst + (rfirst - rit)).base();
                        memory::Construct<T>(block, *rit);
                    }
                    inserted = dst;
                }
                return inserted;
            }
            
            iterator insert(const_iterator dst, iterator first, iterator last)
            {
                iterator inserted;
                if (contains(dst))
                {
                    iterator old_end = end();
                    siz count = last - first;
                    append_capacity(count);
                    _size += count;
                    shift_elements_right(dst, old_end, dst + (last - first), end());
                    memory::Block block{.size = TRAITS::T_SIZE};
                    for (iterator it = first; it != last; it++)
                    {
                        block.ptr = (dst + (it - first)).base();
                        memory::Construct<T>(block, *it);
                    }
                    inserted = dst;
                }
                return inserted;
            }
            
            iterator insert(const_iterator dst, reverse_iterator rfirst, reverse_iterator rlast)
            {
                iterator inserted;
                if (contains(dst))
                {
                    iterator old_end = end();
                    siz count = rfirst - rlast;
                    append_capacity(count);
                    _size += count;
                    shift_elements_right(dst, old_end, dst + (rfirst - rlast), end());
                    memory::Block block{.size = TRAITS::T_SIZE};
                    for (reverse_iterator rit = rfirst; rit != rlast; rit++)
                    {
                        block.ptr = (dst + (rfirst - rit)).base();
                        memory::Construct<T>(block, *rit);
                    }
                    inserted = dst;
                }
                return inserted;
            }
            
            iterator insert(const_iterator dst, init_list<T> list)
            {
                iterator inserted;
                if (contains(dst))
                {
                    iterator old_end = end();
                    append_capacity(list.size());
                    _size += list.size();
                    shift_elements_right(dst, old_end, dst + list.size(), end());
                    memory::Block block{.size = TRAITS::T_SIZE};
                    for (siz i=0; i<list.size(); i++)
                    {
                        block.ptr = (dst + i).base();
                        memory::Construct<T>(block, list[i]);
                    }
                    inserted = dst;
                }
                return inserted;
            }
            
            template <class... Args>
            iterator emplace(const_iterator dst, Args&&... args)
            {
                NP_ASSERT((::std::is_constructible_v<T, Args...>), "T must be constructible with the given Args");
                
                iterator emplaced;
                if (contains(dst) || dst == end())
                {
                    iterator old_end = end();
                    append_capacity(1);
                    _size++;
                    shift_elements_right(dst, old_end, dst + 1, end());
                    memory::Block block{.ptr = dst.base(), .size = TRAITS::T_SIZE};
                    memory::Construct<T>(block, ::std::forward<Args>(args)...);
                }
                
                return emplaced;
            }
            
            template <class... Args>
            reference emplace_back(Args&&... args)
            {
                return *emplace(end(), ::std::forward<Args>(args)...);
            }
            
            iterator erase(const_iterator dst)
            {
                iterator erased;
                if (contains(dst))
                {
                    memory::Destruct<T>(dst.base());
                    siz index = dst - begin();
                    _size--;
                    
                    if (dst - begin() <= (size() - 1) / 2)
                    {
                        //erase from front
                        shift_elements_right(begin(), dst, begin() + 1, dst + 1);
                        
                        if (_begin_index == TRAITS::BUFFER_SIZE - 1)
                        {
                            deallocate_front_buffer();
                            _begin_index = 0;
                        }
                        else
                        {
                            _begin_index++;
                        }
                    }
                    else
                    {
                        //erase from back
                        shift_elements_left(dst + 1, end(), dst, end() - 1);
                        
                        //calc how many buffers we are using
                        siz count = (_begin_index + _size) / TRAITS::BUFFER_SIZE + 1;
                        deallocate_back_buffers(_buffers.size() - count);
                    }
                    
                    erased = begin() + index;
                }
                return erased;
            }
            
            iterator erase(iterator first, iterator last)
            {
                iterator erased;
                if (contains(first) && (contains(last) || last == end()) && first < last)
                {
                    dif diff = last - first;
                    
                    for (iterator it = first; it != last; it++)
                    {
                        memory::Destruct<T>(it.base());
                    }
                    
                    siz index = first - begin();
                    _size -= diff;
                    
                    if (end() - last >= first - begin())
                    {
                        //erase from front
                        shift_elements_right(begin(), first, begin() + diff, last);
                        
                        siz i = _begin_index + diff;
                        siz deallocate_count = i / TRAITS::BUFFER_SIZE;
                        deallocate_front_buffers(deallocate_count);
                        _begin_index = i - deallocate_count * TRAITS::BUFFER_SIZE;
                    }
                    else
                    {
                        //erase from back
                        shift_elements_left(last, end(), first, end() - diff);
                        
                        //calc how many buffers we are using
                        siz count = (_begin_index + _size) / TRAITS::BUFFER_SIZE + 1;
                        deallocate_back_buffers(_buffers.size() - count);
                    }
                    
                    erased = begin() + index;
                }
                return erased;
            }
            
            void push_back(const_reference value)
            {
                emplace_back(value);
            }
            
            void push_back(T&& value)
            {
                emplace_back(value);
            }
            
            void pop_back()
            {
                erase(end() - 1);
            }
            
            void push_front(const_reference value)
            {
                emplace_front(value);
            }
            
            void push_front(T&& value)
            {
                emplace_front(value);
            }
            
            template <class ... Args>
            reference emplace_front(Args&&... args)
            {
                return *emplace(begin(), ::std::forward<Args>(args)...);
            }
            
            void pop_front()
            {
                erase(begin());
            }
            
            void resize(siz count)
            {
                resize(count, T());
            }
            
            void resize(siz count, const_reference value)
            {
                if (size() < count)
                {
                    append(count - size(), value);
                }
                else if (size() > count)
                {
                    erase(begin() + count, end());
                }
            }
            
            //TODO: finish the following:
            
            void swap (deque& other) noexcept
            {
                if (memory::AddressOf(get_allocator()) != memory::AddressOf(other.get_allocator()))
                {
                    allocator_reference allocator = get_allocator();
                    set_allocator(other.get_allocator());
                    other.set_allocator(allocator);
                }
                
                ::std::swap(_begin_index, other._begin_index);
                ::std::swap(_size, other._size);
                ::std::swap(_buffers, other._buffers);
            }
            
            bl operator==(const deque& other) const
            {
                bl equals = _size == other._size;
                for (siz i=0; equals && i<_size; i++)
                {
                    equals = at(i) == other.at(i);
                }
                return equals;
            }
            
            bl operator!=(const deque& other) const
            {
                return !(*this == other);
            }
            
            bl operator<(const deque& other) const
            {
                return ::std::lexicographical_compare(begin(), end(), other.begin(), other.end());
            }
            
            bl operator>(const deque& other) const
            {
                return other < *this;
            }
            
            bl operator<=(const deque& other) const
            {
                return !(other < *this);
            }
            
            bl operator>=(const deque& other) const
            {
                return !(*this < other);
            }
            
            //another erase for the whole deque?? investigate this...
            
        };
    }
}

namespace std
{
    /**
     add ::np::container::deque support to ::std::swap
     */
    template <class T>
    void swap(::np::container::deque<T>& a, ::np::container::deque<T>& b)
    {
        a.swap(b);
    }
}

#endif /* NP_ENGINE_DEQUE_HPP */
