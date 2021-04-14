//
//  vector.hpp
//  Project Space
//
//  Created by Nathan Phipps on 12/14/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_VECTOR_HPP
#define NP_ENGINE_VECTOR_HPP

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
         vector data structure that supports memory::Allocator
         */
        template <class T>
        class vector
        {
        private:
            NP_STATIC_ASSERT(typetraits::IsDefaultConstructible<T>, "T must be default constructible");
            NP_STATIC_ASSERT(typetraits::IsCopyConstructible<T>, "T must be copy constructible");
            NP_STATIC_ASSERT(typetraits::IsMoveConstructible<T>, "T must be move constructible");
            NP_STATIC_ASSERT(typetraits::IsCopyAssignable<T>, "T must be copy assignable");
            NP_STATIC_ASSERT(typetraits::IsMoveAssignable<T>, "T must be move assignable");
            NP_STATIC_ASSERT(typetraits::IsDestructible<T>, "T must be destructible");
            
        public:
            using value_type = T;
            using allocator_type = memory::Allocator;
            using allocator_reference = allocator_type&;
            using allocator_const_reference = const allocator_type&;
            using allocator_pointer = allocator_type*;
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
            
            constexpr static siz MIN_CAPACITY = BIT(2);
            
        private:
            
            constexpr static siz T_SIZE = sizeof(T);
            
            allocator_pointer _allocator;
            siz _capacity;
            pointer _elements;
            siz _size;
            
            /**
             calculates the proper capacity value that supports the given target capacity
             */
            siz calc_capacity(siz target_capacity) const
            {
                siz capacity = 1;
                
                while (capacity < target_capacity)
                {
                    capacity <<= 1;
                }
                
                return capacity;
            }
            
            /**
             ensures that we have capacity for the target capacity
             this only grows the capacity -- call shrink_to_fit or resize to shrink
             */
            bl ensure_capacity(siz target_capacity = MIN_CAPACITY)
            {
                target_capacity = calc_capacity(target_capacity);
                bl ensured = target_capacity <= _capacity;
                
                if (target_capacity > _capacity)
                {
                    memory::Block block = _allocator->Allocate(target_capacity * T_SIZE);
                    if (block.IsValid())
                    {
                        if (memory::ConstructArray<T>(block, target_capacity))
                        {
                            pointer old_elements = _elements;
                            iterator old_begin = begin();
                            iterator old_end = end();
                            
                            _elements = (pointer)block.Begin();
                            move_from(old_begin, old_end);
                            
                            memory::DestructArray<T>(old_elements, _capacity);
                            bl deallocated = _allocator->Deallocate(old_elements);
                            NP_ASSERT(deallocated, "we require a successful deallocation here");
                            
                            _capacity = target_capacity;
                            ensured = true;
                        }
                        else
                        {
                            _allocator->Deallocate(block);
                            NP_ASSERT(false, "construction failed");
                        }
                    }
                    else
                    {
                        NP_ASSERT(false, "our allocation and construction failed");
                    }
                }
                
                return ensured;
            }
            
            /**
             copies the values from given begin and end iterators to our elements
             this sets _size
             */
            void copy_from(const_iterator begin, const_iterator end)
            {
                _size = end - begin;
                
                for (iterator it = begin; it < end; it++)
                {
                    at(it - begin) = *it;
                }
            }
            
            /**
             copies the values from given begin and end reverse iterators to our elements
             this sets _size
             */
            void copy_from(const_reverse_iterator rbegin, const_reverse_iterator rend)
            {
                _size = rbegin - rend;
                
                for (reverse_iterator it = rbegin; it != rend; it++)
                {
                    at(rbegin - it) = *it;
                }
            }
            
            /**
             moves the values from given begin and end iterators to our elements
             this sets _size
             */
            void move_from(const_iterator begin, const_iterator end)
            {
                _size = end - begin;
                
                for (iterator it = begin; it < end; it++)
                {
                    at(it - begin) = utility::Move(*it);
                }
            }
            
            /**
             moves the values from given begin and end reverse iterators to our elements
             this sets _size
             */
            void move_from(const_reverse_iterator rbegin, const_reverse_iterator rend)
            {
                _size = rbegin - rend;
                
                for (reverse_iterator it = rbegin; it != rend; it++)
                {
                    at(rbegin - it) = utility::Move(*it);
                }
            }
            
            /**
             moves the given begin to end range to the given destination in the order from end to begin
             we assume all given iterators are valid and are contained in our maintained elements
             */
            void move_backwards(const_iterator src_begin, const_iterator src_end, const_iterator dst_begin)
            {
                for (reverse_iterator it = src_end - 1; it.base() > src_begin.base(); it++)
                {
                    *(dst_begin + ((iterator)it - src_begin)) = utility::Move(*it);
                }
            }
            
            /**
             moves the given begin to end range to the given destination in the order from begin to end
             we assume all given iterators are valid and are contained in our maintained elements
             */
            void move_forwards(const_iterator src_begin, const_iterator src_end, const_iterator dst_begin)
            {
                for (iterator it = src_begin; it < src_end; it++)
                {
                    *(dst_begin + (it - src_begin)) = utility::Move(*it);
                }
            }
            
            /**
             init with given allocator and capacity
             */
            void init(allocator_reference allocator, siz capacity)
            {
                if (_allocator != nullptr)
                {
                    memory::DestructArray<T>(_elements, _capacity);
                    bl deallocation = _allocator->Deallocate(_elements);
                    NP_ASSERT(deallocation, "deallocation must be successful");
                }
                
                _allocator = memory::AddressOf(allocator);
                _capacity = calc_capacity(capacity);
                init();
            }
            
            /**
             init
             */
            void init()
            {
                if (_capacity < MIN_CAPACITY)
                {
                    _capacity = MIN_CAPACITY;
                }
                
                memory::Block allocation = _allocator->Allocate(_capacity * T_SIZE);
                NP_ASSERT(allocation.IsValid(), "we need successful allocation here in vector.init()");
                
                memory::ConstructArray<T>(allocation, _capacity);
                _elements = (pointer)allocation.Begin();
                
                _size = 0;
            }
            
        public:
            
            /**
             constructor with given allocator and capacity
             */
            vector(allocator_reference allocator, siz capacity):
            _allocator(memory::AddressOf(allocator)),
            _capacity(calc_capacity(capacity))
            {
                init();
            }
            
            /**
             constructor with given allocator
             */
            vector(allocator_reference allocator):
            vector(allocator, MIN_CAPACITY)
            {}
            
            /**
             default constructor
             */
            vector():
            vector(memory::DefaultAllocator, MIN_CAPACITY)
            {}
            
            /**
             constructor with given capacity
             */
            vector(siz capacity):
            vector(memory::DefaultAllocator, capacity)
            {}
            
            /**
             constructor with given allocator and init_list<T>
             */
            vector(allocator_reference allocator, init_list<T> list):
            vector(allocator, list.size())
            {
                copy_from((const_iterator)list.begin(), list.end());
            }
            
            /**
             constructor with given init_list<T>
             */
            vector(init_list<T> list):
            vector(memory::DefaultAllocator, list.size())
            {
                copy_from((const_iterator)list.begin(), list.end());
            }
            
            /**
             copy constructor
             */
            vector(const vector<T>& other):
            vector(*other._allocator, other._capacity)
            {
                copy_from(other.begin(), other.end());
            }
            
            /**
             move constructor - acts like copy on allocator and capacity but moves elements
             */
            vector(vector<T>&& other):
            vector(*other._allocator, other._capacity)
            {
                move_from(other.begin(), other.end());
            }
            
            /**
             copy constructor with specified allocator
             */
            vector(allocator_reference allocator, const vector<T>& other):
            vector(allocator, other._capacity)
            {
                copy_from(other.begin(), other.end());
            }
            
            /**
             move constructor with specified allocator
             */
            vector(allocator_reference allocator, vector<T>&& other):
            vector(allocator, other._capacity)
            {
                move_from(other.begin(), other.end());
            }
            
            /**
             deconstructor
             */
            ~vector()
            {
                memory::DestructArray<T>(_elements, _capacity);
                _allocator->Deallocate(_elements);
            }
            
            /**
             copy assignment
             */
            vector& operator=(const vector<T>& other)
            {
                init(*other._allocator, other._capacity);
                copy_from(other.begin(), other.end());
                return *this;
            }
            
            /**
             move assignment
             */
            vector& operator=(vector<T>&& other)
            {
                init(*other._allocator, other._capacity);
                move_from(other.begin(), other.end());
                return *this;
            }
            
            /**
             assign to given init_list<T>
             */
            vector& operator=(init_list<T> list)
            {
                init(*_allocator, list.size());
                copy_from((const_iterator)list.begin(), list.end());
                return *this;
            }
            
            /**
             assign to given count of given value
             */
            void assign(siz count, const_reference value)
            {
                init(*_allocator, count);
                _size = count;
                for (iterator it = begin(); it != end(); it++)
                {
                    *it = value;
                }
            }
            
            /**
             assign to given init_list<T>
             */
            void assign(init_list<T> list)
            {
                init(*_allocator, list.size());
                copy_from((const_iterator)list.begin(), list.end());
            }
            
            /**
             assign to values given the begin and end iterators
             */
            void assign(iterator begin, iterator end)
            {
                NP_ASSERT(end > begin, "end must be greater than begin for iterator");
                init(*_allocator, end - begin);
                copy_from(begin, end);
            }
            
            /**
             assign to values given the begin and end reverse iterators
             */
            void assign(reverse_iterator rbegin, reverse_iterator rend)
            {
                NP_ASSERT(rbegin > rend, "begin must be greater than end for reverse_iterator");
                init(*_allocator, rbegin - rend);
                copy_from(rbegin, rend);
            }
            
            /**
             gets the allocator reference this vector is using
             */
            allocator_reference get_allocator()
            {
                return *_allocator;
            }
            
            /**
             gets the allocator reference this vector is using
             */
            allocator_const_reference get_allocator() const
            {
                return *_allocator;
            }
            
            /**
             sets the allocator this vector will use
             */
            bl set_allocator(allocator_reference allocator)
            {
                bl set = false;
                memory::Block block = allocator.Allocate(_capacity * T_SIZE);
                
                if (block.IsValid())
                {
                    if (memory::ConstructArray<T>(block, _capacity))
                    {
                        pointer old_elements = _elements;
                        iterator old_begin = begin();
                        iterator old_end = end();
                        
                        _elements = (pointer)block.Begin();
                        move_from(old_begin, old_end); //sets _size
                        
                        memory::DestructArray<T>(old_elements, _capacity);
                        bl deallocated_successful = _allocator->Deallocate(old_elements);
                        NP_ASSERT(deallocated_successful, "we require successful deallocation here");
                        _allocator = &allocator;
                        
                        set = true;
                    }
                    else
                    {
                        allocator.Deallocate(block);
                    }
                }
                
                return set;
            }
            
            /**
             returns the element at the given index
             this does range check
             */
            reference at(siz index)
            {
                NP_ASSERT(0 <= index && index < _size, "index must be [0, size)");
                return _elements[index];
            }
            
            /**
             returns the element at the given index
             this does range check
             */
            const_reference at(siz index) const
            {
                NP_ASSERT(0 <= index && index < size(), "index must be [0, size)");
                return _elements[index];
            }
            
            /**
             subscript operator
             */
            reference operator[](siz index)
            {
                return at(index);
            }
            
            /**
             subscript operator
             */
            const_reference operator[](siz index) const
            {
                return at(index);
            }
            
            /**
             returns the front element
             */
            reference front()
            {
                return at(0);
            }
            
            /**
             returns the front element
             */
            const_reference front() const
            {
                return at(0);
            }
            
            /**
             returns the back element
             */
            reference back()
            {
                return at(size() - 1);
            }
            
            /**
             returns the back element
             */
            const_reference back() const
            {
                return at(size() - 1);
            }
            
            /**
             returns the iterator at the beginning of this vector
             */
            iterator begin()
            {
                return iterator(_elements);
            }
            
            /**
             returns the iterator at the beginning of this vector
             */
            const_iterator begin() const
            {
                return const_iterator(_elements);
            }
            
            /**
             returns the iterator at the end of this vector
             */
            iterator end()
            {
                return iterator(_elements + _size);
            }
            
            /**
             returns the iterator at the end of this vector
             */
            const_iterator end() const
            {
                return const_iterator(_elements + _size);
            }
            
            /**
             returns the reverse iterator at the r beginning of this vector
             */
            reverse_iterator rbegin()
            {
                return reverse_iterator(_elements + _size);
            }
            
            /**
             returns the reverse iterator at the r beginning of this vector
             */
            const_reverse_iterator rbegin() const
            {
                return const_reverse_iterator(_elements + _size);
            }
            
            /**
             returns the reverse iterator at the r end of this vector
             */
            reverse_iterator rend()
            {
                return reverse_iterator(_elements);
            }
            
            /**
             returns the reverse iterator at the r end of this vector
             */
            const_reverse_iterator rend() const
            {
                return const_reverse_iterator(_elements);
            }
            
            /**
             returns the const iterator at the beginning of this vector
             */
            const_iterator cbegin() const
            {
                return begin();
            }
            
            /**
             returns the const iterator at the end of this vector
             */
            const_iterator cend() const
            {
                return end();
            }
            
            /**
             returns the const reverse iterator at the r beginning of this vector
             */
            const_reverse_iterator crbegin() const
            {
                return rbegin();
            }
            
            /**
             returns the const reverse iterator at the r end of this vector
             */
            const_reverse_iterator crend() const
            {
                return rend();
            }
            
            /**
             indicated if this vector is empty or not
             */
            bl empty() const
            {
                return size() == 0;
            }
            
            /**
             returns the size of this vector
             */
            siz size() const
            {
                return _size;
            }
            
            /**
             returns the numeric limit of the siz primitive
             */
            siz max_size() const
            {
                return utility::NumericLimits<siz>::max();
            }
            
            /**
             returns the capacity of this vector
             */
            siz capacity() const
            {
                return _capacity;
            }
            
            /**
             resizes this vector to the given target_size
             */
            bl resize(siz target_size)
            {
                bl resized = false;
                
                if (target_size != _size || _capacity != calc_capacity(_size))
                {
                    siz target_capacity = calc_capacity(target_size); //get capacity to support target size
                    memory::Block block = _allocator->Allocate(target_capacity * T_SIZE);
                    
                    if (block.IsValid())
                    {
                        if (memory::ConstructArray<T>(block, target_capacity))
                        {
                            pointer old_elements = _elements;
                            iterator old_begin = begin();
                            iterator old_end = end();
                            siz min_size = target_size < _size ? target_size : _size;
                            
                            _elements = (pointer)block.Begin();
                            move_from(old_begin, old_begin + min_size); //sets _size
                            _size = target_size; // reset _size here in case we are growing in size
                            
                            memory::DestructArray<T>(old_elements, _capacity);
                            bl deallocated_successful = _allocator->Deallocate(old_elements);
                            NP_ASSERT(deallocated_successful, "we require successful deallocation here");
                            
                            _capacity = target_capacity;
                            resized = true;
                        }
                        else
                        {
                            _allocator->Deallocate(block);
                            NP_ASSERT(false, "construction failed");
                        }
                    }
                    else
                    {
                        _elements = nullptr;
                        NP_ASSERT(false, "we (temporarily) require successful allocation here");
                    }
                }
                else
                {
                    resized = true; //"resize" was successful since we have that target size and the capacity matches
                }
                
                return resized;
            }
            
            /**
             resizes this vector given the target size and if we are growing then we set the "appended" elements to given value
             */
            bl resize(siz target_size, const_reference value)
            {
                bl resized = false;
                
                if (target_size != _size || _capacity != calc_capacity(_size))
                {
                    siz target_capacity = calc_capacity(target_size);
                    memory::Block block = _allocator->Allocate(target_capacity * T_SIZE);
                    
                    if (block.IsValid())
                    {
                        if (memory::ConstructArray<T>(block, target_capacity))
                        {
                            pointer old_elements = _elements;
                            iterator old_begin = begin();
                            iterator old_end = end();
                            bl is_size_decreasing = target_size < _size;
                            siz min_size = is_size_decreasing ? target_size : _size;
                            
                            _elements = (pointer)block.Begin();
                            move_from(old_begin, old_begin + min_size);
                            
                            memory::DestructArray<T>(old_elements, _capacity);
                            bl deallocation_successful = _allocator->Deallocate(old_elements);
                            NP_ASSERT(deallocation_successful, "we require successful deallocation here");
                            
                            _capacity = target_capacity;
                            
                            if (!is_size_decreasing)
                            {
                                iterator old_end = end();
                                _size = target_size;
                                for (iterator it = old_end; it != end(); it++)
                                {
                                    *it = value;
                                }
                            }
                            
                            resized = true;
                        }
                        else
                        {
                            _allocator->Deallocate(block);
                            NP_ASSERT(false, "construction failed");
                        }
                    }
                    else
                    {
                        _elements = nullptr;
                        NP_ASSERT(false, "our allocation failed");
                    }
                }
                else
                {
                    resized = true; //"resize" was successful since we have that target size and the capacity matches
                }
                
                return resized;
            }
            
            /**
             shrinks our capacity to better fit our actual elements
             */
            bl shrink_to_fit()
            {
                return resize(size());
            }
            
            /**
             clears the vector
             */
            void clear() noexcept
            {
                memory::DestructArray<T>(_elements, _capacity);
                _allocator->Deallocate(_elements);
                _capacity = MIN_CAPACITY;
                init();
            }
            
            /**
             indicates if the given iterator points to an element
             */
            bl contains(const_iterator it) const
            {
                return begin() <= it && it < end();
            }
            
            /**
             indicates if the given reverse iterator pointer to an element
             */
            bl contains(const_reverse_iterator it) const
            {
                return begin() <= (iterator)it && (iterator)it < end();
            }
            
            /**
             inserts the given value at the given destination
             the given destination can be [begin, end]
             */
            iterator insert(const_iterator dst, const_reference value)
            {
                iterator inserted = nullptr;
                if (contains(dst) || dst == end())
                {
                    siz index = dst - begin();
                    if (ensure_capacity(_size + 1))
                    {
                        inserted = begin() + index;
                        move_backwards(inserted, end(), inserted + 1);
                        *inserted = value;
                        _size++;
                    }
                }
                return inserted;
            }
            
            /**
             inserts the given count of given value at the given destination
             the given destination can be [begin, end]
             */
            iterator insert(const_iterator dst, siz count, const_reference value)
            {
                iterator inserted = nullptr;
                if ((contains(dst) || dst == end()) && count > 0)
                {
                    siz index = dst - begin();
                    if (ensure_capacity(_size + count))
                    {
                        inserted = begin() + index;
                        move_backwards(inserted, end(), inserted + count);
                        
                        for (iterator it = inserted; it < inserted + count; it++)
                        {
                            *it = value;
                        }
                        
                        _size += count;
                    }
                }
                return inserted;
            }
            
            /**
             inserts the given values via given begin to end reverse iterators at the given destination
             the given destination can be [begin, end]
             */
            iterator insert(const_iterator dst, reverse_iterator src_rbegin, reverse_iterator src_rend)
            {
                iterator inserted = nullptr;
                if ((contains(dst) || dst == end()) && src_rbegin > src_rend)
                {
                    siz index = dst - begin();
                    siz src_size = src_rbegin - src_rend;
                    if (ensure_capacity(_size + src_size))
                    {
                        inserted = begin() + index;
                        move_backwards(inserted, end(), inserted + src_size);
                        
                        for (reverse_iterator it = src_rbegin; it != src_rend; it++)
                        {
                            *(inserted + (src_rbegin - it)) = (const_reference)*it;
                        }
                        
                        _size += src_size;
                    }
                }
                return inserted;
            }
            
            /**
             inserts the given values via given begin to end iterators at the given destination
             the given destination can be [begin, end]
             */
            iterator insert(const_iterator dst, iterator src_begin, iterator src_end)
            {
                iterator inserted = nullptr;
                if ((contains(dst) || dst == end()) && src_end > src_begin)
                {
                    siz index = dst - begin();
                    siz src_size = src_end - src_begin;
                    if (ensure_capacity(_size + src_size))
                    {
                        inserted = begin() + index;
                        move_backwards(inserted, end(), inserted + src_size);
                        
                        for (iterator it = src_begin; it < src_end; it++)
                        {
                            *(inserted + (it - src_begin)) = (const_reference)*it;
                        }
                        
                        _size += src_size;
                    }
                }
                return inserted;
            }
            
            /**
             inserts the given init_list<T> at the given destination
             the given destination can be [begin, end]
             */
            iterator insert(const_iterator dst, init_list<T> list)
            {
                iterator inserted = nullptr;
                if ((contains(dst) || dst == end()) && list.size() > 0)
                {
                    siz index = dst - begin();
                    if (ensure_capacity(size() + list.size()))
                    {
                        inserted = begin() + index;
                        move_backwards(inserted, end(), inserted + list.size());
                        
                        for (iterator it = list.begin(); it < (iterator)list.end(); it++)
                        {
                            *(inserted + (it - (iterator)list.begin())) = (const_reference)*it;
                        }
                        
                        _size += list.size();
                    }
                }
                return inserted;
            }
            
            /**
             erases the element at the given iterator
             */
            iterator erase(const_iterator it)
            {
                iterator erased = nullptr;
                
                if (contains(it))
                {
                    move_forwards(it + 1, end(), it);
                    _size--;
                    /*
                    memory::Block block{.ptr = (pointer)end(), .size = T_SIZE};
                    memory::Destruct<T>(block.ptr);
                    memory::Construct<T>(block);
                     */
                    erased = it;
                }
                
                return erased;
            }
            
            /**
             erases the elements from the given begin to end iterators
             */
            iterator erase(const_iterator src_begin, const_iterator src_end)
            {
                iterator erased = nullptr;
                
                if (contains(src_begin) && (contains(src_end) || src_end == end()) && src_end > src_begin)
                {
                    move_forwards(src_end, end(), src_begin);
                    //iterator old_end = end();
                    _size -= src_end - src_begin;
                    /*
                    memory::Block block{.size = T_SIZE};
                    for (iterator it = end(); it != old_end; it++)
                    {
                        block.ptr = (pointer)it;
                        memory::Destruct<T>(block.ptr);
                        memory::Construct<T>(block);
                    }
                     */
                    erased = src_begin;
                }
                
                return erased;
            }
            
            /**
             constructs an element at the given destination
             destination can be [begin, end]
             */
            template <class... Args>
            iterator emplace(const_iterator dst, Args&&... args)
            {
                iterator emplaced = nullptr;
                if (contains(dst) || dst == end())
                {
                    siz index = dst - begin();
                    if (ensure_capacity(_size + 1))
                    {
                        emplaced = begin() + index;
                        move_backwards(emplaced, end(), emplaced + 1);
                        memory::Block block{.ptr = emplaced, .size = T_SIZE};
                        
                        if (memory::Destruct<T>(emplaced) &&
                            memory::Construct<T>(block, utility::Forward<Args>(args)...))
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
            
            /**
             constructs an element at the end
             */
            template <class... Args>
            iterator emplace_back(Args&&... args)
            {
                return emplace(end(), utility::Forward<Args>(args)...);
            }
            
            /**
             pushes an element at the end
             */
            iterator push_back(const_reference value)
            {
                return emplace_back(value);
            }
            
            /**
             pushes an element at the end
             */
            iterator push_back(T&& value)
            {
                return emplace_back(utility::Move(value));
            }
            
            /**
             pops and element at the end
             */
            void pop_back()
            {
                _size--;
            }
            
            /**
             swaps vectors
             */
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

