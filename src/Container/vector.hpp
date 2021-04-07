//
//  vector.hpp
//  Project Space
//
//  Created by Nathan Phipps on 12/14/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_VECTOR_HPP
#define NP_ENGINE_VECTOR_HPP

#include <vector>
#include <limits>

#include "Primitive/Primitive.hpp"
#include "Utility/Utility.hpp"
#include "Memory/Memory.hpp"

#include "iterator.hpp"
#include "init_list.hpp"

namespace np
{
    namespace container
    {
        /**
         brings the std vector here since it is so useful
         */
        template <class T>
        using std_vector = ::std::vector<T>;
                
        //TODO: refactor our methods to match std snake casing...
        
        //TODO: add summary comments
        
        /**
         vector data structure that supports memory::Allocator
         */
        template <class T>
        class vector
        {
        public:
            using value_type = T;
            using allocator_type = memory::Allocator; //TODO: should we enable this?
            using size_type = siz;
            using difference_type = dif;
            
            using iterator = container::iterator<T*>;
            using const_iterator = const container::iterator<T*>;
            using reference = typename iterator::reference;
            using const_reference = typename iterator::const_reference;
            using pointer = typename iterator::pointer;
            using const_pointer = typename iterator::const_pointer;
            
            using reverse_iterator = container::reverse_iterator<T*>;
            using const_reverse_iterator = const container::reverse_iterator<T*>;
            
        private:
            
            constexpr static siz T_SIZE = sizeof(T);
            
            memory::Allocator* _allocator;
            siz _capacity;
            pointer _elements;
            siz _size;
            
            siz CalcCapacity(siz target_capacity) const
            {
                siz capacity = 1;
                
                while (capacity < target_capacity)
                {
                    capacity <<= 1;
                }
                
                return capacity;
            }
            
            bl EnsureCapacity(siz target_capacity = 0)
            {
                target_capacity = target_capacity != 0 ? target_capacity : _capacity + 1;
                siz next_capacity = CalcCapacity(target_capacity);
                bl ensured = next_capacity <= _capacity;
                
                if (next_capacity > _capacity)
                {
                    memory::Block next_block = _allocator->Allocate(next_capacity);
                    if (next_block.IsValid())// && memory::ConstructArray<T>(next_block, next_capacity))
                    {
                        pointer old_elements = _elements;
                        iterator old_begin = begin();
                        iterator old_end = end();
                        
                        _elements = (pointer)next_block.Begin();
                        MoveFrom(old_begin, old_end);
                        
                        Destroy(old_begin, old_end);
                        _allocator->Deallocate(old_elements);
                        
                        _capacity = next_capacity;
                        ensured = true;
                    }
                    else
                    {
                        _elements = nullptr;
                        NP_ASSERT(false, "our allocation failed");
                    }
                }
                
                return ensured;
            }
            
            void CopyFrom(const_iterator begin, const_iterator end)
            {
                memory::Block block{.size = T_SIZE};
                for (iterator it = begin; it < end; it++)
                {
                    block.ptr = _elements + (it - begin);
                    memory::Construct<T>(block, *it);
                }
                
                _size = end - begin;
            }
            
            void CopyFrom(const_reverse_iterator rbegin, const_reverse_iterator rend)
            {
                memory::Block block{.size = T_SIZE};
                for (reverse_iterator it = rbegin; it != rend; it++)
                {
                    block.ptr = _elements + (it - rend);
                    memory::Construct<T>(block, *it);
                }
                
                _size = rbegin - rend;
            }
            
            void MoveFrom(const_iterator begin, const_iterator end)
            {
                memory::Block block{.size = T_SIZE};
                for (iterator it = begin; it < end; it++)
                {
                    block.ptr = _elements + (it - begin);
                    memory::Construct<T>(block, utility::Move(*it));
                }
                
                _size = end - begin;
            }
            
            void MoveFrom(const_reverse_iterator rbegin, const_reverse_iterator rend)
            {
                memory::Block block{.size = T_SIZE};
                for (reverse_iterator it = rbegin; it != rend; it++)
                {
                    block.ptr = _elements + (it - rend);
                    memory::Construct<T>(block, utility::Move(*it));
                }
                
                _size = rbegin - rend;
            }
            
            
            //TODO: ensure our move backwards / forwards actually work
            void MoveBackwards(const_iterator src_begin, const_iterator src_end, const_iterator dst_begin)
            {
                //TODO: use reverse_iterator
                memory::Block block{.size = T_SIZE};
                for (iterator src_it(src_end - 1); src_it >= src_begin; src_it--)
                {
                    block.ptr = dst_begin + (src_it - src_begin);
                    memory::Construct<T>(block, utility::Move(*src_it));
                }
//                reverse_iterator src_rbegin((pointer)src_end);
//                reverse_iterator src_rend((pointer)src_begin);
//
//                memory::Block block{.size = T_SIZE};
//                for (reverse_iterator src = src_rbegin; src != src_rend; src++)
//                {
//                    block.ptr = dst_begin + (src - src_begin);
//                    memory::Construct<T>(block, utility::Move(*src));
//                }
            }
            
            void MoveForwards(const_iterator src_begin, const_iterator src_end, const_iterator dst_begin)
            {
                memory::Block block{.size = T_SIZE};
                for (iterator src_it(src_begin); src_it < src_end; src_it++)
                {
                    block.ptr =  dst_begin + (src_it - src_begin);
                    memory::Construct<T>(block, utility::Move(*src_it));
                }
            }
            
            void Destroy(iterator it)
            {
                memory::Destruct<T>(it);
            }
            
            void Destroy(iterator begin, iterator end)
            {
                for (iterator it = begin; it != end; it++)
                {
                    Destroy(it);
                }
            }
            
            void Init(memory::Allocator* allocator, siz capacity)
            {
                if (_allocator != nullptr)
                {
                    Destroy(begin(), end());
                    bl deallocation = _allocator->Deallocate(_elements);
                    NP_ASSERT(deallocation, "deallocation must be successful");
                    
//                    _elements = nullptr;
//                    _size = 0;
//                    _allocator = nullptr;
                }

                _allocator = allocator;
                _capacity = CalcCapacity(capacity);
                Init();
            }
            
            void Init()
            {
                memory::Block allocation = _allocator->Allocate(_capacity * T_SIZE);
                NP_ASSERT(allocation.IsValid(), "we need successful allocation here in vector.Init()");
                _elements = (pointer)allocation.Begin();
                _size = 0;
            }
            
        public:
            
            vector(memory::Allocator* allocator, siz capacity):
            _allocator(allocator),
            _capacity(CalcCapacity(capacity))
            {
                Init();
            }
            
            vector(memory::Allocator* allocator):
            vector(allocator, 1)
            {}
            
            vector():
            vector(&memory::DefaultAllocator, 1)
            {}
            
            vector(siz capacity):
            vector(&memory::DefaultAllocator, capacity)
            {}
            
            vector(memory::Allocator* allocator, init_list<T> list):
            vector(allocator, list.size())
            {
                CopyFrom((const_iterator)list.begin(), list.end());
            }
            
            vector(init_list<T> list):
            vector(&memory::DefaultAllocator, list.size())
            {
                CopyFrom((const_iterator)list.begin(), list.end());
            }
            
            vector(const vector<T>& other):
            vector(other._allocator, other._capacity)
            {
                CopyFrom(other.begin(), other.end());
            }
            
            vector(vector<T>&& other):
            vector(other._allocator, other._capacity)
            {
                MoveFrom(other.begin(), other.end());
            }
            
            vector(memory::Allocator* allocator, const vector<T>& other):
            vector(allocator, other._capacity)
            {
                CopyFrom(other.begin(), other.end());
            }
            
            vector(memory::Allocator* allocator, vector<T>&& other):
            vector(allocator, other._capacity)
            {
                MoveFrom(other.begin(), other.end());
            }
            
            ~vector()
            {
                Destroy(begin(), end());
                _allocator->Deallocate(_elements);
            }
            
            vector& operator=(const vector<T>& other)
            {
                Init(other._allocator, other._capacity);
                CopyFrom(other.begin(), other.end());
                return *this;
            }
            
            vector& operator=(vector<T>&& other)
            {
                Init(other._allocator, other._capacity);
                MoveFrom(other.begin(), other.end());
                return *this;
            }
            
            vector& operator=(init_list<T> list)
            {
                Init(_allocator, list.size());
                CopyFrom((const_iterator)list.begin(), list.end());
                return *this;
            }
                        
            void assign(siz count, const_reference value)
            {
                Init(_allocator, count);
                memory::Block block{.size = T_SIZE};
                for (siz i=0; i<count; i++)
                {
                    block.ptr = _elements + i;
                    memory::Construct<T>(block, value);
                }
                _size = count;
            }
            
            void assign(init_list<T> list)
            {
                Init(_allocator, list.size());
                CopyFrom((const_iterator)list.begin(), list.end());
            }
            
            void assign(iterator begin, iterator end)
            {
                NP_ASSERT(end > begin, "end must be greater than begin for iterator");
                Init(_allocator, end - begin);
                CopyFrom(begin, end);
            }
            
            void assign(reverse_iterator rbegin, reverse_iterator rend)
            {
                NP_ASSERT(rbegin > rend, "begin must be greater than end for reverse_iterator");
                Init(_allocator, rbegin - rend);
                CopyFrom(rbegin, rend);
            }
            
            memory::Allocator& get_allocator()
            {
                return *_allocator;
            }
            
            const memory::Allocator& get_allocator() const
            {
                return *_allocator;
            }
            
            bl set_allocator(memory::Allocator* allocator)
            {
                bl set = false;
                
                //TODO: implement set_allocator
                NP_ASSERT(false, "not implemented");
                
                return set;
            }
            
            reference at(siz index)
            {
                NP_ASSERT(0 <= index && index < size(), "index must be [0, size)");
                return _elements[index];
            }
            
            const_reference at(siz index) const
            {
                NP_ASSERT(0 <= index && index < size(), "index must be [0, size)");
                return _elements[index];
            }
            
            
            reference operator[](siz index)
            {
                return at(index);
            }
            
            const_reference operator[](siz index) const
            {
                return at(index);
            }
            
            //front
            reference front()
            {
                return at(0);
            }
            
            const_reference front() const
            {
                return at(0);
            }
            
            //back
            reference back()
            {
                return at(size() - 1);
            }
            
            const_reference back() const
            {
                return at(size() - 1);
            }
            
            //begin end
            
            iterator begin()
            {
                return iterator(_elements);
            }
            
            const_iterator begin() const
            {
                return const_iterator(_elements);
            }
            
            iterator end()
            {
                return iterator(_elements + _size);
            }
            
            const_iterator end() const
            {
                return const_iterator(_elements + _size);
            }
            
            //rbegin rend
            
            reverse_iterator rbegin()
            {
                return reverse_iterator(end());
            }
            
            const_reverse_iterator rbegin() const
            {
                return const_reverse_iterator(end());
            }
            
            reverse_iterator rend()
            {
                return reverse_iterator(begin());
            }
            
            const_reverse_iterator rend() const
            {
                return const_reverse_iterator(begin());
            }
            
            //cbegin cend
            
            const_iterator cbegin() const
            {
                return begin();
            }
            
            const_iterator cend() const
            {
                return end();
            }
            
            //crbegin crend
            
            const_reverse_iterator crbegin() const
            {
                return rbegin();
            }
            
            const_reverse_iterator crend() const
            {
                return rend();
            }
            
            //empty
            
            bl empty() const
            {
                return size() == 0;
            }
            
            //size
            
            siz size() const
            {
                return _size;
            }
            
            //max_size
            
            siz max_size() const
            {
                return ::std::numeric_limits<siz>::max(); //TODO: we should probably have this in utility?
            }
            
            siz capacity() const
            {
                return _capacity;
            }
            
            bl resize(siz target_size)
            {
                //TODO: implement resize
                bl resized = false;
                
                if (target_size != size())
                {
                    siz target_capacity = CalcCapacity(target_size);
                    memory::Block target_block = _allocator->Allocate(target_capacity * T_SIZE);
                    
                    if (target_block.IsValid())
                    {
                        siz min_size = size() < target_capacity ? size() : target_capacity;
                        pointer old_elements = _elements;
                        iterator old_begin = begin();
                        iterator old_end = end();
                        
                        _elements = (pointer)target_block.Begin();
                        _capacity = target_capacity;
                        MoveFrom(old_begin, old_begin + min_size);
                        
                        Destroy(old_begin, old_end);
                        _allocator->Deallocate(old_elements);
                        resized = true;
                    }
                    else
                    {
                        _elements = nullptr;
                        NP_ASSERT(false, "our allocation failed");
                    }
                }
                
                return resized;
            }
            
            bl resize(siz target_size, const_reference value)
            {
                //TODO: implement resize(siz, const T&)
                bl resized = false;
                siz target_capacity = CalcCapacity(target_size);
                memory::Block target_block = _allocator->Allocate(target_capacity * T_SIZE);
                
                if (target_block.IsValid())
                {
                    bl is_increasing_capacity = target_capacity > size();
                    siz min_size = is_increasing_capacity ? size() : target_capacity;
                    pointer old_elements = _elements;
                    iterator old_begin = begin();
                    iterator old_end = end();
                    
                    _elements = (pointer)target_block.Begin();
                    _capacity = target_capacity;
                    MoveFrom(old_begin, old_begin + min_size);
                    
                    Destroy(old_begin, old_end);
                    _allocator->Deallocate(old_elements);
                    
                    if (is_increasing_capacity)
                    {
                        memory::Block block{.size = T_SIZE};
                        iterator old_end = end();
                        _size = target_size;
                        for (iterator it = old_end; it < end(); it++)
                        {
                            block.ptr = (pointer)it;
                            memory::Construct<T>(block, value);
                        }
                    }
                    
                    resized = true;
                }
                else
                {
                    _elements = nullptr;
                    NP_ASSERT(false, "our allocation failed");
                }
                
                return resized;
            }
            
            
            //shrink_to_fit
            void shrink_to_fit()
            {
                resize(size());
            }
            
            //clear
            void clear() noexcept
            {
                Destroy(begin(), end());
                _allocator->Deallocate(_elements);
                _capacity = 1;
                Init();
            }
            
            bl contains(const_iterator it) const
            {
                return begin() <= it && it < end();
            }
            
            bl contains(const_reverse_iterator it) const
            {
                return begin() <= it && it < end();
            }
            
            //TODO: insert
            iterator insert(const_iterator it, const_reference value)
            {
                iterator inserted = nullptr;
                bl it_is_end = it == end();
                if (contains(it) || it_is_end)
                {
                    siz index = it - begin();
                    if (EnsureCapacity(size() + 1))
                    {
                        inserted = begin() + index;
                        MoveBackwards(inserted, end(), inserted + 1);
                        memory::Block block{.ptr = inserted, .size = T_SIZE};
                        
                        if (!it_is_end)
                        {
                            memory::Destruct<T>((pointer)block.ptr);
                        }
                        
                        if (memory::Construct<T>(block, value))
                        {
                            _size++;
                        }
                        else
                        {
                            inserted = nullptr;
                        }
                    }
                }
                return inserted;
            }
            
            iterator insert(const_iterator it, siz count, const_reference value)
            {
                iterator inserted = it;
                bl it_is_end = it == end();
                if ((contains(it) || it_is_end) && count > 0)
                {
                    siz index = it - begin();
                    if (EnsureCapacity(size() + count))
                    {
                        inserted = begin() + index;
                        MoveBackwards(inserted, end(), inserted + count);
                        {
                            memory::Block block{.size = T_SIZE};
                            for (siz i = 0; i < count; i++)
                            {
                                block.ptr = inserted + i;
                                if (!it_is_end)
                                {
                                    memory::Destruct<T>((pointer)block.ptr);
                                }
                                memory::Construct<T>(block, value);
                            }
                            
                            _size += count;
                        }
                    }
                }
                return inserted;
            }
            
            //TODO: support reverse_iterators
            
            iterator insert(const_iterator dst_it, reverse_iterator src_rbegin, reverse_iterator src_rend)
            {
                iterator inserted = dst_it;
                bl it_is_end = dst_it == end();
                if ((contains(dst_it) || it_is_end) && src_rbegin > src_rend)
                {
                    siz index = dst_it - begin();
                    if (EnsureCapacity(size() + (src_rbegin - src_rend)))
                    {
                        inserted = begin() + index;
                        MoveBackwards(inserted, end(), inserted + (src_rbegin - src_rend));
                        {
                            memory::Block block{.size = T_SIZE};
                            for (iterator i = src_rend; i != src_rbegin; i++)
                            {
                                block.ptr = inserted + (i - src_rend);
                                if (!it_is_end)
                                {
                                    memory::Destruct<T>((pointer)block.ptr);
                                }
                                memory::Construct<T>(block, const_reference(*i));
                            }
                            
                            _size += src_rbegin - src_rend;
                        }
                    }
                }
                return inserted;
            }
            
            iterator insert(const_iterator dst_it, iterator src_begin, iterator src_end)
            {
                iterator inserted = dst_it;
                bl it_is_end = dst_it == end();
                if ((contains(dst_it) || it_is_end) && src_end > src_begin)
                {
                    siz index = dst_it - cbegin();
                    if (EnsureCapacity(size() + (src_end - src_begin)))
                    {
                        inserted = begin() + index;
                        MoveBackwards(inserted, end(), inserted + (src_end - src_begin));
                        {
                            memory::Block block{.size = T_SIZE};
                            for (iterator i = src_begin; i != src_end; i++)
                            {
                                block.ptr = inserted + (i - src_begin);
                                if (!it_is_end)
                                {
                                    memory::Destruct<T>((pointer)block.ptr);
                                }
                                memory::Construct<T>(block, const_reference(*i));
                            }
                            
                            _size += src_end - src_begin;
                        }
                    }
                }
                return inserted;
            }
            
            iterator insert(const_iterator it, init_list<T> list)
            {
                iterator inserted = it;
                bl it_is_end = it == end();
                if ((contains(it) || it_is_end) && list.size() > 0)
                {
                    siz index = it - begin();
                    if (EnsureCapacity(size() + list.size()))
                    {
                        inserted = begin() + index;
                        MoveBackwards(inserted, end(), inserted + list.size());
                        {
                            memory::Block block{.size = T_SIZE};
                            for (siz i = 0; i < list.size(); i++)
                            {
                                block.ptr = inserted + i;
                                if (!it_is_end)
                                {
                                    memory::Destruct<T>((pointer)block.ptr);
                                }
                                memory::Construct<T>(block, const_reference(list[i]));
                            }
                            
                            _size += list.size();
                        }
                    }
                }
                return inserted;
            }
            
            //TODO: erase
            //TODO: do we need to check about calling shrink_to_fit during an erase??
            
            iterator erase(const_iterator it)
            {
                iterator erased = nullptr;
                
                if (contains(it))
                {
                    Destroy(it);
                    MoveForwards(it + 1, end(), it);
                    _size--;
                    erased = it;
                }
                
                return erased;
            }
            
            iterator erase(const_reverse_iterator src_rbegin, const_reverse_iterator src_rend)
            {
                iterator erased = nullptr;
                
                if (contains(src_rend) && (contains(src_rbegin) || src_rbegin == end()) && src_rbegin > src_rend)
                {
                    Destroy(src_rend, src_rbegin);
                    MoveForwards(src_rbegin + 1, end(), src_rend);
                    _size -= src_rbegin - src_rend;
                    erased = src_rend;
                }
                
                return erased;
            }
            
            iterator erase(const_iterator src_begin, const_iterator src_end)
            {
                iterator erased = nullptr;
                
                if (contains(src_begin) && (contains(src_end) || src_end == end()) && src_end > src_begin)
                {
                    Destroy(src_begin, src_end);
                    MoveForwards(src_end + 1, end(), src_begin);
                    _size -= src_end - src_begin;
                    erased = src_begin;
                }
                
                return erased;
            }
            
            //TODO: emplace
            
            template <class... Args>
            iterator emplace(const_iterator it, Args&&... args)
            {
                iterator emplaced = nullptr;
                bl it_is_end = it == end();
                if (contains(it) || it_is_end)
                {
                    siz index = it - begin();
                    if (EnsureCapacity(size() + 1))
                    {
                        emplaced = begin() + index;
                        MoveBackwards(emplaced, end(), emplaced + 1);
                        memory::Block block{.ptr = emplaced, .size = T_SIZE};
                        
                        if (!it_is_end)
                        {
                            memory::Destruct<T>((pointer)block.ptr);
                        }
                        
                        if (memory::Construct<T>(block, utility::Forward<Args>(args)...))
                        {
                            _size++;
                        }
                        else
                        {
                            emplaced = nullptr;
                        }
                    }
                }
                
                return emplaced;
            }
            
            //TODO: emplace_back
            
            template <class... Args>
            iterator emplace_back(Args&&... args)
            {
                memory::Block block{.size = T_SIZE};
                if (EnsureCapacity(size() + 1))
                {
                    block.ptr = end();
                    if (memory::Construct<T>(block, utility::Forward<Args>(args)...))
                    {
                        _size++;
                    }
                    else
                    {
                        block.Invalidate();
                    }
                }
                else
                {
                    block.Invalidate();
                }
                
                return block.Begin();
            }
            
            //TODO: push_back
            
            iterator push_back(const_reference value)
            {
                memory::Block block{.size = T_SIZE};
                if (EnsureCapacity(size() + 1))
                {
                    block.ptr = end();
                    if (memory::Construct<T>(block, value))
                    {
                        _size++;
                    }
                    else
                    {
                        block.Invalidate();
                    }
                }
                else
                {
                    block.Invalidate();
                }
                
                return block.Begin();
            }
            
            iterator push_back(T&& value)
            {
                memory::Block block{.size = T_SIZE};
                if (EnsureCapacity(size() + 1))
                {
                    block.ptr = end();
                    if (memory::Construct<T>(block, utility::Move(value)))
                    {
                        _size++;
                    }
                    else
                    {
                        block.Invalidate();
                    }
                }
                else
                {
                    block.Invalidate();
                }
                
                return block.Begin();
            }
            
            //TODO: pop_back
            
            void pop_back()
            {
                Destroy(end() - 1);
                _size--;
            }
            
            //TODO: swap
            
            void swap(vector<T>& other) noexcept
            {
                utility::Swap(_allocator, other._allocator);
                utility::Swap(_capacity, other._capacity);
                utility::Swap(_elements, other._elements);
                utility::Swap(_size, other._size);
            }
        };
    }
}

#endif /* NP_ENGINE_VECTOR_HPP */
