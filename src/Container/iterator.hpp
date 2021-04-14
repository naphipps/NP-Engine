//
//  iterator.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 3/23/21.
//

#ifndef NP_ENGINE_ITERATOR_HPP
#define NP_ENGINE_ITERATOR_HPP

#include <iterator>

#include "TypeTraits/TypeTraits.hpp"

namespace np
{
    namespace container
    {
        /**
         represents the normal pointer iterator that we all know
         */
        template <class T>
        struct iterator
        {
            NP_STATIC_ASSERT(typetraits::IsPointer<T>, "T must be a pointer");
            
            using value_type = typetraits::RemovePointerObject<T>;
            using pointer = value_type*;
            using const_pointer = const value_type*;
            using reference = value_type&;
            using const_reference = const value_type&;
            using difference_type = dif;
            using iterator_category = ::std::random_access_iterator_tag;
            
            pointer ptr;
            
            /**
             constructor
             */
            iterator():
            ptr(nullptr)
            {}
            
            /**
             constructor
             */
            iterator(const iterator& other):
            ptr(other.ptr)
            {}
            
            /**
             constructor
             */
            iterator(iterator&& other):
            ptr(utility::Move(other.ptr))
            {}
            
            /**
             constructor for void*
             */
            iterator(const void* t):
            ptr((pointer)t)
            {}
            
            /**
             constructor for nullptr
             */
            iterator(nptr):
            ptr(nullptr)
            {}
            
            /**
             deconstructor
             */
            ~iterator() = default;
            
            /**
             base method returns our ptr
             */
            pointer base()
            {
                return ptr;
            }
            
            /**
             base method returns our ptr
             */
            const_pointer base() const
            {
                return ptr;
            }
            
            /**
             cast operator allows us to implicitly convert to T
             */
            operator T()
            {
                return ptr;
            }
            
            /**
             copy assignment operator
             */
            iterator& operator=(const iterator& other)
            {
                ptr = other.ptr;
                return *this;
            }
            
            /**
             move assignment operator
             */
            iterator& operator=(iterator&& other)
            {
                ptr = utility::Move(other.ptr);
                return *this;
            }
            
            /**
             assignment operator for void*
             */
            iterator& operator=(const void* t)
            {
                ptr = (pointer)t;
                return *this;
            }
            
            /**
             assignment operator for nullptr
             */
            iterator& operator=(nptr)
            {
                ptr = nullptr;
                return *this;
            }
            
            /**
             pointer to member access operator
             */
            reference operator*() const
            {
                return *ptr;
            }
            
            /**
             pointer to member access operator
             */
            pointer operator->() const
            {
                return memory::AddressOf(operator*());
            }
            
            /**
             subscript operator
             */
            reference operator[](dif n) const
            {
                return *(ptr + n);
            }
            
            /**
             pre increment operator
             */
            iterator& operator++()
            {
                ++ptr;
                return *this;
            }
            
            /**
             post increment operator
             */
            iterator operator++(i32)
            {
                return iterator(ptr++);
            }
            
            /**
             pre increment operator
             */
            iterator& operator--()
            {
                --ptr;
                return *this;
            }
            
            /**
             post increment operator
             */
            iterator operator--(i32)
            {
                return iterator(ptr--);
            }
            
            /**
             increment operator
             */
            iterator& operator+=(dif n)
            {
                ptr += n;
                return *this;
            }
            
            /**
             decrement operator
             */
            iterator& operator-=(dif n)
            {
                ptr -= n;
                return *this;
            }
            
            /**
             addition operator with dif
             */
            iterator operator+(dif n) const
            {
                return iterator(ptr + n);
            }
            
            /**
             subtraction operator with dif
             */
            iterator operator-(dif n) const
            {
                return iterator(ptr - n);
            }
            
            /**
             subtraction operator with other iterator
             */
            dif operator-(const iterator& other) const
            {
                return ptr - other.ptr;
            }
            
            /**
             is equals operator
             */
            bl operator==(const iterator& other) const
            {
                return ptr == other.ptr;
            }
            
            /**
             is not equals operator
             */
            bl operator!=(const iterator& other) const
            {
                return ptr != other.ptr;
            }
            
            /**
             is less than operator
             */
            bl operator<(const iterator& other) const
            {
                return ptr < other.ptr;
            }
            
            /**
             is greater than operator
             */
            bl operator>(const iterator& other) const
            {
                return ptr > other.ptr;
            }
            
            /**
             is less than or equal to operator
             */
            bl operator<=(const iterator& other) const
            {
                return ptr <= other.ptr;
            }
            
            /**
             is greater than or equak to operator
             */
            bl operator>=(const iterator& other) const
            {
                return ptr >= other.ptr;
            }
        };
        
        /**
         represents the reverse of our iterator struct
         */
        template <class T>
        struct reverse_iterator
        {
            NP_STATIC_ASSERT(typetraits::IsPointer<T>, "T must be a pointer");
            
            using value_type = typetraits::RemovePointerObject<T>;
            using pointer = value_type*;
            using const_pointer = const value_type*;
            using reference = value_type&;
            using const_reference = const value_type&;
            using difference_type = dif;
            using iterator_category = ::std::random_access_iterator_tag;
            
            pointer ptr;
            
            /**
             constructor
             */
            reverse_iterator():
            ptr(nullptr)
            {}
            
            /**
             copy constructor
             */
            reverse_iterator(const reverse_iterator& other):
            ptr(other.ptr)
            {}
            
            /**
             move constructor
             */
            reverse_iterator(reverse_iterator&& other):
            ptr(utility::Move(other.ptr))
            {}
            
            /**
             copy constructor
             */
            reverse_iterator(const iterator<T>& other):
            ptr(other.ptr + 1)
            {}
            
            /**
             move constructor
             */
            reverse_iterator(iterator<T>&& other):
            ptr(other.ptr + 1)
            {}
            
            /**
             copy constructor - copies std::reverse_iterator
             */
            reverse_iterator(const ::std::reverse_iterator<pointer>& other):
            ptr(other.base())
            {}
            
            /**
             move constructor - moves std::reverse_iterator
             */
            reverse_iterator(::std::reverse_iterator<pointer>&& other):
            ptr(utility::Move(other.base()))
            {}
            
            /**
             constructor for void*
             */
            reverse_iterator(const void* t):
            ptr((pointer)t)
            {}
            
            /**
             constructor for nullptr
             */
            reverse_iterator(nptr):
            ptr(nullptr)
            {}
            
            /**
             deconstructor
             */
            ~reverse_iterator() = default;
            
            /**
             base method returns our ptr
             */
            pointer base()
            {
                return ptr;
            }
            
            /**
             base method returns our ptr
             */
            const_pointer base() const
            {
                return ptr;
            }
            
            /**
             cast operator allows us to implicitly convert to T
             */
            operator T()
            {
                return ptr;
            }
            
            operator iterator<T>()
            {
                return iterator<T>(ptr - 1);
            }
            
            /**
             cast operator allows us to implicitly convert to std::reverse_iterator
             */
            operator ::std::reverse_iterator<T>()
            {
                return ::std::reverse_iterator<T>(ptr);
            }
            
            /**
             copy assignment operator
             */
            reverse_iterator& operator=(const reverse_iterator& other)
            {
                ptr = other.ptr;
                return *this;
            }
            
            /**
             move assignment operator
             */
            reverse_iterator& operator=(reverse_iterator&& other)
            {
                ptr = utility::Move(other.ptr);
                return *this;
            }
            
            /**
             copy assignment operator
             */
            reverse_iterator& operator=(const iterator<T>& other)
            {
                ptr = other.ptr + 1;
                return *this;
            }
            
            /**
             move assignment operator
             */
            reverse_iterator& operator=(iterator<T>&& other)
            {
                ptr = other.ptr + 1;
                return *this;
            }
            
            /**
             copy assignment operator - copies std::reverse_iterator
             */
            reverse_iterator& operator=(const ::std::reverse_iterator<pointer>& other)
            {
                ptr = other.base();
                return *this;
            }
            
            /**
             move assignment operator - moves std::reverse_iterator
             */
            reverse_iterator& operator=(::std::reverse_iterator<pointer>&& other)
            {
                ptr = utility::Move(other.base());
                return *this;
            }
            
            /**
             assignment operator for void*
             */
            reverse_iterator& operator=(const void* t)
            {
                ptr = (pointer)t;
                return *this;
            }
            
            /**
             assignment operator for nullptr
             */
            reverse_iterator& operator=(nptr)
            {
                ptr = nullptr;
                return *this;
            }
            
            /**
             pointer to member operator
             */
            reference operator*() const
            {
                return *(ptr - 1);
            }
            
            /**
             pointer to member operator
             */
            pointer operator->() const
            {
                return memory::AddressOf(operator*());
            }
            
            /**
             subscript operator
             */
            reference operator[](dif n) const
            {
                return *(ptr - 1 - n);
            }
            
            /**
             pre increment operator
             */
            reverse_iterator& operator++()
            {
                --ptr;
                return *this;
            }
            
            /**
             post increment operator
             */
            reverse_iterator operator++(i32)
            {
                return reverse_iterator(ptr--);
            }
            
            /**
             pre decrement operator
             */
            reverse_iterator& operator--()
            {
                ++ptr;
                return *this;
            }
            
            /**
             post increment operator
             */
            reverse_iterator operator--(i32)
            {
                return reverse_iterator(ptr++);
            }
            
            /**
             increment operator
             */
            reverse_iterator& operator+=(dif n)
            {
                ptr -= n;
                return *this;
            }
            
            /**
             decrement operator
             */
            reverse_iterator& operator-=(dif n)
            {
                ptr += n;
                return *this;
            }
            
            /**
             addition operator with dif
             */
            reverse_iterator operator+(dif n) const
            {
                return reverse_iterator(ptr - n);
            }
            
            /**
             subtraction operator with dif
             */
            reverse_iterator operator-(dif n) const
            {
                return reverse_iterator(ptr + n);
            }
            
            /**
             subtraction operator with other reverse_iterator
             */
            dif operator-(const reverse_iterator& other) const
            {
                return ptr - other.ptr;
            }
            
            /**
             is equal operator
             */
            bl operator==(const reverse_iterator& other) const
            {
                return ptr == other.ptr;
            }
            
            /**
             is not equal operator
             */
            bl operator!=(const reverse_iterator& other) const
            {
                return ptr != other.ptr;
            }
            
            /**
             is less than operator
             */
            bl operator<(const reverse_iterator& other) const
            {
                return ptr < other.ptr;
            }
            
            /**
             is greater than operator
             */
            bl operator>(const reverse_iterator& other) const
            {
                return ptr > other.ptr;
            }
            
            /**
             is less than or equal operator
             */
            bl operator<=(const reverse_iterator& other) const
            {
                return ptr <= other.ptr;
            }
            
            /**
             is greater than or equal operator
             */
            bl operator>=(const reverse_iterator& other) const
            {
                return ptr >= other.ptr;
            }
        };
    }
}

#endif /* NP_ENGINE_ITERATOR_HPP */
