//
//  Color.hpp
//  Project Space
//
//  Created by Nathan Phipps on 12/12/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef COLOR_HPP
#define COLOR_HPP

#include "Primitive/Primitive.hpp"

namespace np
{
    namespace core
    {
        /**
         represents color given the 32 bit i value split up into 8 bit rgba
         */
        struct Color //TODO: I think we should move this somewhere else... maybe that Image project?
        {
            union
            {
                ui32 i;
                struct
                {
                    ui8 r;
                    ui8 g;
                    ui8 b;
                    ui8 a;
                };
            };
        };
    }
}

#endif /* COLOR_HPP */
