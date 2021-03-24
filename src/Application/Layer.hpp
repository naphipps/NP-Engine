//
//  Layer.hpp
//  Project Space
//
//  Created by Nathan Phipps on 2/9/21.
//  Copyright © 2021 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_APPLICATION_LAYER_HPP
#define NP_ENGINE_APPLICATION_LAYER_HPP

#include "Primitive/Primitive.hpp"
#include "Container/Container.hpp"
#include "Time/Time.hpp"
#include "Event/Event.hpp"

namespace np
{
    namespace app
    {
        /**
         Layer will represent a layer in our application
         */
        class Layer
        {
        public:
            
            /**
             constructor
             */
            Layer() = default;
            
            /**
             deconstructor
             */
            virtual ~Layer() = default;
            
            /**
             attach method
             */
            virtual void Attach()
            {}
            
            /**
             detach method
             */
            virtual void Detach()
            {}

            /**
             update method receives a time delta value
             */
            virtual void Update(time::DurationMilliseconds time_delta)
            {}
            
            /**
             on event method
             */
            virtual void OnEvent(event::Event& e)
            {}
        };
    }
}

#endif /* NP_ENGINE_APPLICATION_LAYER_HPP */
