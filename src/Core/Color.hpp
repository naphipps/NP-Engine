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
#include "Memory/Memory.hpp"

namespace np
{
    namespace core
    {
        /**
         represents color given the 32 bit i value split up into 8 bit rgba
         */
        struct Color //TODO: I think we should move this somewhere else... maybe that Image project?
        {
            //TODO: how else would we represent this without punning?? make the i portion a function, since we really only care about the rgba - we could make RgbColor, RgbaColor, HsvColor, etc
            
            ui8 r;
            ui8 g;
            ui8 b;
            ui8 a;
            
            ui32 ToUi32() const
            {
                ui32 i;
                memory::CopyBytes(&i, &r, sizeof(ui32)); //TODO: fix this...
                return i;
            }
            
            void FromUi32(ui32 i)
            {
                r = (i >> 24) & 255;
                g = (i >> 16) & 255;
                b = (i >> 8) & 255;
                a = i & 255;
            }
        };
    }
}

#endif /* COLOR_HPP */
