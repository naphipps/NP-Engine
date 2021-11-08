//
//  deque_iterator.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 7/27/21.
//

#ifndef NP_ENGINE_DEQUE_ITERATOR_HPP
#define NP_ENGINE_DEQUE_ITERATOR_HPP

#include <type_traits>
#include <utility>

#include "NP-Engine/Insight/Insight.hpp"

#include "container_traits.hpp"
#include "iterator.hpp" //::std::random_access_iterator_tag

namespace np::container
{
    //we want our deque_iterator to friend the following:
    template <class T>
    class deque;
    
    template <class T>
    class deque_reverse_iterator;
    
    /**
     the iterator used for our deque container
     */
    template <class T>
    class deque_iterator
    {
    private:
        friend class deque<::std::remove_pointer_t<T>>;
        friend class deque_reverse_iterator<T>;
        
    public:
        NP_STATIC_ASSERT(::std::is_pointer_v<T>, "T must be a pointer");
        
        using attached_deque_type = deque<T>;
        using value_type = ::std::remove_pointer_t<T>;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using difference_type = dif;
        using iterator_category = ::std::random_access_iterator_tag;
        
    private:
        using TRAITS = deque_traits<T>;
        using buffer_iterator = typename vector<pointer>::iterator;
        
        buffer_iterator _buffers;
        pointer _ptr;
        
        /**
         copy from given deque_iterator
         */
        void copy_from(const deque_iterator& other)
        {
            _buffers = other._buffers;
            _ptr = other._ptr;
        }
        
        /**
         move from given deque_iterator
         */
        void move_from(deque_iterator&& other)
        {
            _buffers = ::std::move(other._buffers);
            _ptr = ::std::move(other._ptr);
        }
        
        /**
         deque_iterator constructor used only by our deque container and deque_reverse_iterator
         */
        deque_iterator(buffer_iterator buffers, pointer ptr):
        _buffers(buffers),
        _ptr(ptr)
        {}
        
    public:
        
        /**
         constructor
         */
        deque_iterator():
        _buffers(nullptr),
        _ptr(nullptr)
        {}
        
        /**
         constructor
         */
        deque_iterator(const deque_iterator& other)
        {
            copy_from(other);
        }
        
        /**
         constructor
         */
        deque_iterator(deque_iterator&& other)
        {
            move_from(::std::move(other));
        }
        
        /**
         constructor for nptr
         */
        deque_iterator(nptr):
        deque_iterator()
        {}
        
        /**
         base method returns our ptr
         */
        pointer base()
        {
            return _ptr;
        }
        
        /**
         base method returns our ptr
         */
        const_pointer base() const
        {
            return _ptr;
        }
        
        /**
         cast operator allows us to implicitly convert to T
         */
        operator T()
        {
            return _ptr;
        }
        
        /**
         cast operator allows us to implicitly convert to const T
         */
        operator const T() const //TODO: test this
        {
            return _ptr;
        }
        
        /**
         copy assignment
         */
        deque_iterator& operator=(const deque_iterator& other)
        {
            copy_from(other);
            return *this;
        }
        
        /**
         move assignment
         */
        deque_iterator& operator=(deque_iterator&& other)
        {
            move_from(other);
            return *this;
        }
        
        /**
         assignment operator for nullptr
         */
        deque_iterator& operator=(nptr)
        {
            _buffers = nullptr;
            _ptr = nullptr;
            return *this;
        }
        
        /**
         pointer to member access operator
         */
        reference operator*() const
        {
            return *_ptr;
        }
        
        /**
         pointer to member access operator
         */
        pointer operator->() const
        {
            return _ptr;
        }
        
        /**
         subscript operator
         */
        reference operator[](dif n) const
        {
            return *(*this + n);
        }
        
        /**
         pre increment operator
         */
        deque_iterator& operator++()
        {
            return *this += 1;
        }
        
        /**
         post increment operator
         */
        deque_iterator operator++(i32)
        {
            deque_iterator it = *this;
            *this += 1;
            return it;
        }
        
        /**
         pre decrement operator
         */
        deque_iterator& operator--()
        {
            return *this -= 1;
        }
        
        /**
         post decrement operator
         */
        deque_iterator operator--(i32)
        {
            deque_iterator it = *this;
            *this -= 1;
            return *this;
        }
        
        /**
         increment operator
         */
        deque_iterator& operator+=(dif n)
        {
            if (n != 0)
            {
                n += _ptr - *_buffers; //if > 0, then we're inc forward, else dec backward
                if (n > 0)
                {
                    _buffers += n / TRAITS::BUFFER_SIZE;
                    _ptr = *_buffers + n % TRAITS::BUFFER_SIZE;
                }
                else
                {
                    n = TRAITS::BUFFER_SIZE - 1 - n;
                    _buffers -= n / TRAITS::BUFFER_SIZE;
                    _ptr = *_buffers + (TRAITS::BUFFER_SIZE - 1 - n % TRAITS::BUFFER_SIZE);
                }
            }
            
            return *this;
        }
        
        /**
         decrement operator
         */
        deque_iterator& operator-=(dif n)
        {
            return *this += -n;
        }
        
        /**
         addition operator with dif
         */
        deque_iterator operator+(dif n) const
        {
            deque_iterator it = *this;
            it += n;
            return it;
        }
        
        /**
         subtraction operator with dif
         */
        deque_iterator operator-(dif n) const
        {
            deque_iterator it = *this;
            it -= n;
            return it;
        }
        
        /**
         subtraction operator with other deque_iterator
         */
        dif operator-(const deque_iterator& other) const
        {
            dif n = 0;
            if (*this != other)
            {
                n = (_buffers - other._buffers) * TRAITS::BUFFER_SIZE
                + (_ptr - *_buffers)
                - (other._ptr - *other._buffers);
            }
            return n;
        }
        
        /**
         equals operator
         */
        bl operator==(const deque_iterator& other) const
        {
            return _ptr == other._ptr;
        }
        
        /**
         not equals operator
         */
        bl operator!=(const deque_iterator& other) const
        {
            return _ptr != other._ptr;
        }
        
        /**
         less than operator
         */
        bl operator<(const deque_iterator& other) const
        {
            return _buffers < other._buffers ||
            (_buffers == other._buffers && _ptr < other._ptr);
        }
        
        /**
         greater than operator
         */
        bl operator>(const deque_iterator& other) const
        {
            return other < *this;
        }
        
        /**
         less than or equal to operator
         */
        bl operator<=(const deque_iterator& other) const
        {
            return !(other < *this);
        }
        
        /**
         greater than or equal to operator
         */
        bl operator>=(const deque_iterator& other) const
        {
            return !(*this < other);
        }
    };
    
    //TODO: test deque_reverse_iterator
    //TODO: add comments

    template <class T>
    class deque_reverse_iterator
    {
    private:
        friend class deque<T>;
        
    public:
        NP_STATIC_ASSERT(::std::is_pointer_v<T>, "T must be a pointer");
        
        using attached_deque_type = deque<T>;
        using value_type = ::std::remove_pointer_t<T>;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using difference_type = dif;
        using iterator_category = ::std::random_access_iterator_tag;
        
    private:
        
        using TRAITS = deque_traits<T>;
        
        typename container::vector<pointer>::iterator _buffers;
        pointer _ptr;
        
        void copy_from(const deque_reverse_iterator& other)
        {
            _buffers = other._buffers;
            _ptr = other._ptr;
        }
        
        void copy_from(const deque_iterator<T>& other)
        {
            deque_iterator<T> temp(other);
            temp++;
            _buffers = temp._buffers;
            _ptr = temp._ptr;
        }
        
        void move_from(deque_reverse_iterator&& other)
        {
            _buffers = ::std::move(other._buffers);
            _ptr = ::std::move(other._ptr);
        }
        
        void move_from(deque_iterator<T>&& other)
        {
            _buffers = ::std::move(other._buffers);
            _ptr = ::std::move(other._ptr);
            (*this)--; //TODO: does our normal iterators follow this??
        }
        
        //we only want our deque iterator to be created by our deques
        deque_reverse_iterator(pointer* buffers, pointer ptr):
        _buffers(buffers),
        _ptr(ptr)
        {}
        
    public:
        
        deque_reverse_iterator():
        _buffers(nullptr),
        _ptr(nullptr)
        {}
        
        deque_reverse_iterator(const deque_reverse_iterator& other)
        {
            copy_from(other);
        }
        
        deque_reverse_iterator(deque_reverse_iterator&& other)
        {
            move_from(other);
        }
        
        deque_reverse_iterator(const deque_iterator<T>& other)
        {
            copy_from(other);
        }
        
        deque_reverse_iterator(deque_iterator<T>&& other)
        {
            move_from(other);
        }
        
        deque_reverse_iterator(nptr):
        deque_reverse_iterator()
        {}
        
        pointer base()
        {
            return _ptr;
        }
        
        const_pointer base() const
        {
            return _ptr;
        }
        
        operator T()
        {
            return _ptr;
        }
        
        operator const T() const //TODO: test this -- and if successful, then bring to iterator.hpp
        {
            return _ptr;
        }
        
        operator deque_iterator<T>()
        {
            deque_reverse_iterator temp((const_reference)*this);
            temp++;
            return deque_iterator<T>(temp._buffers, temp._ptr); //TODO: is this correct?
        }
        
        deque_reverse_iterator& operator=(const deque_reverse_iterator& other)
        {
            copy_from(other);
            return *this;
        }
        
        deque_reverse_iterator& operator=(deque_reverse_iterator&& other)
        {
            move_from(other);
            return *this;
        }
        
        deque_reverse_iterator& operator=(const deque_iterator<T>& other)
        {
            copy_from(other);
            return *this;
        }
        
        deque_reverse_iterator& operator=(deque_iterator<T>&& other)
        {
            move_from(other);
            return *this;
        }
        
        deque_reverse_iterator& operator=(nptr)
        {
            _buffers = nullptr;
            _ptr = nullptr;
            return *this;
        }
        
        reference operator*() const
        {
            return *(_ptr - 1);
        }
        
        pointer operator->() const
        {
            return _ptr - 1;
        }
        
        reference operator[](dif n) const
        {
            return *(*this + n);
        }
        
        deque_reverse_iterator& operator++()
        {
            return *this += 1;
        }
        
        deque_reverse_iterator operator++(i32)
        {
            deque_reverse_iterator it = *this;
            *this += 1;
            return it;
        }
        
        deque_reverse_iterator& operator--()
        {
            return *this -= 1;
        }
        
        deque_reverse_iterator operator--(i32)
        {
            deque_iterator it = *this;
            *this -= 1;
            return *this;
        }
        
        deque_reverse_iterator& operator-=(dif n)
        {
            if (n != 0)
            {
                n += _ptr - *_buffers; //if > 0, then we're inc forward, else dec backward
                if (n > 0)
                {
                    _buffers += n / TRAITS::BUFFER_SIZE;
                    _ptr = *_buffers + n % TRAITS::BUFFER_SIZE;
                }
                else
                {
                    n = TRAITS::BUFFER_SIZE - 1 - n;
                    _buffers -= n / TRAITS::BUFFER_SIZE;
                    _ptr = *_buffers + (TRAITS::BUFFER_SIZE - 1 - n % TRAITS::BUFFER_SIZE);
                }
            }
            
            return *this;
        }
        
        deque_reverse_iterator& operator+=(dif n)
        {
            return *this -= -n;
        }
        
        deque_reverse_iterator operator+(dif n) const
        {
            deque_reverse_iterator it = *this;
            it += n;
            return it;
        }
        
        deque_reverse_iterator operator-(dif n) const
        {
            deque_reverse_iterator it = *this;
            it -= n;
            return it;
        }
        
        dif operator-(const deque_reverse_iterator& other) const
        {
            dif n = 0;
            if (*this != other)
            {
                n = (_buffers - other._buffers) * TRAITS::BUFFER_SIZE
                + (_ptr - *_buffers)
                - (other._ptr - *other._buffers);
            }
            return n;
        }
        
        bl operator==(const deque_reverse_iterator& other) const
        {
            return _ptr == other._ptr;
        }
        
        bl operator!=(const deque_reverse_iterator& other) const
        {
            return _ptr != other._ptr;
        }
        
        bl operator>(const deque_reverse_iterator& other) const
        {
            return _buffers < other._buffers ||
            (_buffers == other._buffers && _ptr < other._ptr);
        }
        
        bl operator<(const deque_reverse_iterator& other) const
        {
            return other < *this;
        }
        
        bl operator<=(const deque_reverse_iterator& other) const
        {
            return !(other < *this);
        }
        
        bl operator>=(const deque_reverse_iterator& other) const
        {
            return !(*this < other);
        }
    };
}

#endif /* NP_ENGINE_DEQUE_ITERATOR_HPP */
