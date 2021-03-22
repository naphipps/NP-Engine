//
//  JobFunction.hpp
//  Project Space
//
//  Created by Nathan Phipps on 9/3/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_JOB_FUNCTION_HPP
#define NP_ENGINE_JOB_FUNCTION_HPP

#include "Primitive/Primitive.hpp"
#include "Concurrency/Concurrency.hpp"
#include "Insight/Insight.hpp"

namespace np
{
    namespace js
    {
        /**
         JobFunction function acts as a delegate for member methods and static functions following the void(*)() signature
         */
        class JobFunction// : public concurrency::PaddedObject
        {
        private:
            using InstancePtr = void*;
            using FunctionPtr = void(*)(InstancePtr, JobFunction&);
            
            InstancePtr _instance_ptr;
            FunctionPtr _function_ptr;
            concurrency::PadObject _pad;
            
            /**
             used to wrap a static function to our function pointer
             */
            template <void(*Function)(JobFunction&)>
            inline static void wrap(InstancePtr, JobFunction& function)
            {
                return (Function)(function);
            }
            
            /**
             used to wrap a class method to our function pointer
             */
            template <class C, void(C::*Function)(JobFunction&)>
            inline static void wrap(InstancePtr instance, JobFunction& function)
            {
                return (static_cast<C*>(instance)->*Function)(function);
            }
            
            /**
             copies from the given other JobFunction function
             */
            inline void CopyFrom(const JobFunction& other)
            {
                _instance_ptr = other._instance_ptr;
                _function_ptr = other._function_ptr;
                _pad = other._pad;
            }
            
        public:
            
            /**
             constructor - NULL instance and function pointers
             */
            JobFunction():
            _instance_ptr(NULL),
            _function_ptr(NULL)
            {}
            
            /**
             copy constructor
             */
            JobFunction(const JobFunction& other)
            {
                CopyFrom(other);
            }
            
            /**
             move constructor acts like copy
             */
            JobFunction(JobFunction&& other)
            {
                CopyFrom(other);
            }
            
            /**
             deconstructor
             */
            ~JobFunction()
            {}
            
            /**
             connects a static function to our function pointer
             */
            template <void (*Function)(JobFunction&)>
            inline void Connect()
            {
                _instance_ptr = NULL;
                _function_ptr = &wrap<Function>;
            }
            
            /**
             connects a class method to our function pointer
             */
            template <class C, void(C::*Method)(JobFunction&)>
            inline void Connect(InstancePtr instance)
            {
                _instance_ptr = instance;
                _function_ptr = &wrap<C, Method>;
            }
            
            /**
             invokes our function if valid
             */
            inline void Invoke(JobFunction& function)
            {
                if (IsValid())
                {
                    _function_ptr(_instance_ptr, function);
                }
            }
                        
            /**
             checks if our function pointer is NULL or not
             */
            bl IsValid() const
            {
                return _function_ptr != NULL;
            }
            
            /**
             invalidates our instance and function pointers by setting them to NULL
             */
            inline void Invalidate()
            {
                _instance_ptr = NULL;
                _function_ptr = NULL;
            }
            
            /**
             checks if our function pointer is NULL or not
             */
            inline explicit operator bl() const
            {
                return IsValid();
            }
            
            /**
             call operator calls our invoke method
             */
            inline void operator()(JobFunction& function)
            {
                Invoke(function);
            }
            
            /**
             copy assignment
             */
            inline JobFunction& operator=(const JobFunction& other)
            {
                CopyFrom(other);
                return *this;
            }
            
            /**
             move assignment - act like copy assignment
             */
            JobFunction& operator=(JobFunction&& other)
            {
                CopyFrom(other);
                return *this;
            }
            
            /**
             assigns the given value or struct into the padding of this JobFunction
             */
            template <typename T>
            void AssignData(const T& data)
            {
                _pad.AssignData(data);
            }
            
            /**
             retrieves the value or struct from the padding of this JobFunction
             */
            template <typename T>
            const T& RetrieveData() const
            {
                return _pad.RetrieveData<T>();
            }
        };
    }
}

#endif /* NP_ENGINE_JOB_FUNCTION_HPP */
