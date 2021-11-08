//
//  RandomSeed.hpp
//  Project Space
//
//  Created by Nathan Phipps on 12/7/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_RANDOM_SEED_HPP
#define NP_ENGINE_RANDOM_SEED_HPP

#include <type_traits>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "PcgRandutils.hpp"

namespace np
{
    namespace random
    {
        /**
         RandomSeed shall be used to store the interal state of a random_engine
         */
        template <typename T>
        class RandomSeed
        {
        private:
            
            NP_STATIC_ASSERT((
                ::std::is_same_v<T, ui64> ||
                ::std::is_same_v<T, ::pcg_extras::pcg128_t>),
                "RandomSeed<T> requires T to be ui64 or ::pcg_extras::pcg128_t");
            
            T _inc;
            T _state;
            
            /**
             forces an odd value on inc
             */
            void ForceOddInc()
            {
                _inc |= 1;
            }
            
            /**
             copies from other seed
             */
            void CopyFrom(const RandomSeed<T>& other)
            {
                _inc = other._inc;
                _state = other._state;
            }
            
        public:
            
            /**
             constructor
             */
            RandomSeed()
            {}
            
            /**
             constructor
             */
            RandomSeed(T inc, T state):
            _inc(inc),
            _state(state)
            {}
            
            /**
             copy constructor
             */
            RandomSeed(const RandomSeed<T>& seed)
            {
                CopyFrom(seed);
            }
            
            /**
             move constructor - acts like copy
             */
            RandomSeed(RandomSeed<T>&& seed)
            {
                CopyFrom(seed);
            }
            
            /**
             deconstructor
             */
            ~RandomSeed()
            {}
            
            /**
             copy assignment
             */
            RandomSeed<T>& operator=(const RandomSeed<T>& other)
            {
                CopyFrom(other);
                return *this;
            }
            
            /**
             move assignment - acts like copy
             */
            RandomSeed<T>& operator=(RandomSeed<T>&& other)
            {
                CopyFrom(other);
                return *this;
            }
            
            /**
             gets the inc value
             */
            T GetInc() const
            {
                return _inc;
            }
            
            /**
             gets the state value
             */
            T GetState() const
            {
                return _state;
            }
            
            /**
             sets the inc value for this seed
             this always forces the given value to be odd by or-ing it with 1
             */
            void SetInc(T inc_)
            {
                _inc = inc_;
                ForceOddInc();
            }
            
            /**
             sets the state value for this seed
             */
            void SetState(T state_)
            {
                _state = state_;
            }
        };
    }
}

#endif /* NP_ENGINE_RANDOM_SEED_HPP */
