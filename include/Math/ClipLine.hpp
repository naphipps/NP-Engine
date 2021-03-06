//
//  Created by Nathan Phipps on 3/7/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_CLIP_LINE_HPP
#define NP_ENGINE_CLIP_LINE_HPP

#include "Primitive/Primitive.hpp"
#include "Insight/Insight.hpp"

#include "Point.hpp"

namespace np
{
    namespace math
    {
        /*
         Implementation of the Sobkow-Pospisil-Yang clipping algorithm, with fixed clipping functions,
         and a return struct providing more insight.
         
         Citation:
         Sobkow, M., Pospisila, P., & Yang, Y. (1987). A fast two-dimesional line clipping algorithm via line encoding. Compuing and Graphics, 11(4), 459-467
         */
        
        namespace _hidden
        {
            /**
             clips given point p given top value
             */
            static inline bl ClipPTop(fltPoint &p, fltPoint &q, const flt top)
            {
                fltPoint temp = p;
                p.x += (q.x - p.x) * (top - p.y) / (q.y - p.y);
                p.y = top;
                return temp != p;
            }
            
            /**
             clips given point p given bottom value
             */
            static inline bl ClipPBottom(fltPoint &p, fltPoint &q, const flt bottom)
            {
                fltPoint temp = p;
                p.x += (q.x - p.x) * (bottom - p.y) / (q.y - p.y);
                p.y = bottom;
                return temp != p;
            }
            
            /**
             clips given point p given right value
             */
            static inline bl ClipPRight(fltPoint &p, fltPoint &q, const flt right)
            {
                fltPoint temp = p;
                p.y += (q.y - p.y) * (right - p.x) / (q.x - p.x);
                p.x = right;
                return temp != p;
            }
            
            /**
             clips given point p given left value
             */
            static inline bl ClipPLeft(fltPoint &p, fltPoint &q, const flt left)
            {
                fltPoint temp = p;
                p.y += (q.y - p.y) * (left - p.x) / (q.x - p.x);
                p.x = left;
                return temp != p;
            }
            
            /**
             clips given point q given top value
             */
            static inline bl ClipQTop(fltPoint &p, fltPoint &q, const flt top)
            {
                fltPoint temp = q;
                q.x += (p.x - q.x) * (top - q.y) / (p.y - q.y);
                q.y = top;
                return temp != q;
            }
            
            /**
             clips given point q given bottom value
             */
            static inline bl ClipQBottom(fltPoint &p, fltPoint &q, const flt bottom)
            {
                fltPoint temp = q;
                q.x += (p.x - q.x) * (bottom - q.y) / (p.y - q.y);
                q.y = bottom;
                return temp != q;
            }
            
            /**
             clips given point q given right value
             */
            static inline bl ClipQRight(fltPoint &p, fltPoint &q, const flt right)
            {
                fltPoint temp = q;
                q.y += (p.y - q.y) * (right - q.x) / (p.x - q.x);
                q.x = right;
                return temp != q;
            }
            
            /**
             clips given point q given left value
             */
            static inline bl ClipQLeft(fltPoint &p, fltPoint &q, const flt left)
            {
                fltPoint temp = q;
                q.y += (p.y - q.y) * (left - q.x) / (p.x - q.x);
                q.x = left;
                return temp != q;
            }
        }
        
        /**
         return struct for method ClipLine. Indicates if the line was clipped and is line is visible within given bounds
         */
        struct ClipLineReturn
        {
            bl visible = false;
            bl clipped = false;
        };
        
        /**
         clips given line pq given top, right, bottom, and left value
         */
        static inline ClipLineReturn ClipLine(fltPoint &p, fltPoint &q,
                                       ui16 top, ui16 right, ui16 bottom, ui16 left)
        {
            ClipLineReturn retval;
            
            /*
             line_code key
             --------------------------
             |   $9   |   $8   |   $A   |
             |  1001  |  1000  |  1010  |
             |--------+--------+--------|
             |   $1   |   $0   |   $2   |
             |  0001  |  0000  |  0010  |
             |--------+--------+--------|
             |   $5   |   $4   |   $6   |
             |  0101  |  0100  |  0110  |
             --------------------------
             */
            
            ui8 line_code = 0;
            line_code += q.y > top ? 8 : 0;
            line_code += q.y < bottom ? 4 : 0;
            line_code += q.x > right ? 2 : 0;
            line_code += q.x < left ? 1 : 0;
            line_code += p.y > top ? 128 : 0;
            line_code += p.y < bottom ? 64 : 0;
            line_code += p.x > right ? 32 : 0;
            line_code += p.x < left ? 16 : 0;
            
            switch (line_code)
            {
                case 0x00:
                    retval.visible = true;
                    break;
                    
                case 0x01:
                    retval.clipped = _hidden::ClipQLeft(p, q, left);
                    retval.visible = true;
                    break;
                    
                case 0x02:
                    retval.clipped = _hidden::ClipQRight(p, q, right);
                    retval.visible = true;
                    break;
                    
                case 0x04:
                    retval.clipped = _hidden::ClipQBottom(p, q, bottom);
                    retval.visible = true;
                    break;
                    
                case 0x05:
                    retval.clipped = _hidden::ClipQLeft(p, q, left);
                    if (q.y < bottom)
                    {
                        retval.clipped |= _hidden::ClipQBottom(p, q, bottom);
                    }
                    retval.visible = true;
                    break;
                    
                case 0x06:
                    retval.clipped = _hidden::ClipQRight(p, q, right);
                    if (q.y < bottom)
                    {
                        retval.clipped |= _hidden::ClipQBottom(p, q, bottom);
                    }
                    retval.visible = true;
                    break;
                    
                case 0x08:
                    retval.clipped = _hidden::ClipQTop(p, q, top);
                    retval.visible = true;
                    break;
                    
                case 0x09:
                    retval.clipped = _hidden::ClipQLeft(p, q, left);
                    if (q.y > top)
                    {
                        retval.clipped |= _hidden::ClipQTop(p, q, top);
                    }
                    retval.visible = true;
                    break;
                    
                case 0x0A:
                    retval.clipped = _hidden::ClipQRight(p, q, right);
                    if (q.y > top)
                    {
                        retval.clipped |= _hidden::ClipQTop(p, q, top);
                    }
                    retval.visible = true;
                    break;
                    
                    //---------------------------------------------
                    
                case 0x10:
                    retval.clipped = _hidden::ClipPLeft(p, q, left);
                    retval.visible = true;
                    break;
                    
                case 0x11:
                    break;
                    
                case 0x12:
                    retval.clipped = _hidden::ClipPLeft(p, q, left);
                    retval.clipped |= _hidden::ClipQRight(p, q, right);
                    retval.visible = true;
                    break;
                    
                case 0x14:
                    retval.clipped = _hidden::ClipPLeft(p, q, left);
                    if (p.y < bottom)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipQBottom(p, q, bottom);
                        retval.visible = true;
                    }
                    break;
                    
                case 0x15:
                    break;
                    
                case 0x16:
                    retval.clipped = _hidden::ClipPLeft(p, q, left);
                    if (p.y < bottom)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipQBottom(p, q, bottom);
                        if (q.x > right)
                        {
                            retval.clipped |= _hidden::ClipQRight(p, q, right);
                        }
                        retval.visible = true;
                    }
                    break;
                    
                case 0x18:
                    retval.clipped = _hidden::ClipPLeft(p, q, left);
                    if (p.y > top)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipQTop(p, q, top);
                        retval.visible = true;
                    }
                    break;
                    
                case 0x19:
                    break;
                    
                case 0x1A:
                    retval.clipped = _hidden::ClipPLeft(p, q, left);
                    if (p.y > top)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipQTop(p, q, top);
                        if (q.x > right)
                        {
                            retval.clipped |= _hidden::ClipQRight(p, q, right);
                        }
                        retval.visible = true;
                    }
                    break;
                    
                    //---------------------------------------------
                    
                case 0x20:
                    retval.clipped = _hidden::ClipPRight(p, q, right);
                    retval.visible = true;
                    break;
                    
                case 0x21:
                    retval.clipped = _hidden::ClipPRight(p, q, right);
                    retval.clipped |= _hidden::ClipQLeft(p, q, left);
                    retval.visible = true;
                    break;
                    
                case 0x22:
                    break;
                    
                case 0x24:
                    retval.clipped = _hidden::ClipPRight(p, q, right);
                    if (p.y < bottom)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipQBottom(p, q, bottom);
                        retval.visible = true;
                    }
                    break;
                    
                case 0x25:
                    retval.clipped = _hidden::ClipPRight(p, q, right);
                    if (p.y < bottom)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipQBottom(p, q, bottom);
                        if (q.x < left)
                        {
                            retval.clipped |= _hidden::ClipQLeft(p, q, left);
                        }
                        retval.visible = true;
                    }
                    break;
                    
                case 0x26:
                    break;
                    
                case 0x28:
                    retval.clipped = _hidden::ClipPRight(p, q, right);
                    if (p.y > top)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipQTop(p, q, top);
                        retval.visible = true;
                    }
                    break;
                    
                case 0x29:
                    retval.clipped = _hidden::ClipPRight(p, q, right);
                    if (p.y > top)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipQTop(p, q, top);
                        if (q.x < left)
                        {
                            retval.clipped |= _hidden::ClipQLeft(p, q, left);
                        }
                        retval.visible = true;
                    }
                    break;
                    
                case 0x2A:
                    break;
                    
                    //---------------------------------------------
                    
                case 0x40:
                    retval.clipped = _hidden::ClipPBottom(p, q, bottom);
                    retval.visible = true;
                    break;
                    
                case 0x41:
                    retval.clipped = _hidden::ClipPBottom(p, q, bottom);
                    if (p.x < left)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipQLeft(p, q, left);
                        if (q.y < bottom)
                        {
                            retval.clipped |= _hidden::ClipQBottom(p, q, bottom);
                        }
                        retval.visible = true;
                    }
                    break;
                    
                case 0x42:
                    retval.clipped = _hidden::ClipPBottom(p, q, bottom);
                    if (p.x > right)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipQRight(p, q, right);
                        retval.visible = true;
                    }
                    break;
                    
                case 0x44:
                    break;
                    
                case 0x45:
                    break;
                    
                case 0x46:
                    break;
                    
                case 0x48:
                    retval.clipped = _hidden::ClipPBottom(p, q, bottom);
                    retval.clipped |= _hidden::ClipQTop(p, q, top);
                    retval.visible = true;
                    break;
                    
                case 0x49:
                    retval.clipped = _hidden::ClipPBottom(p, q, bottom);
                    if (p.x < left)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipQLeft(p, q, left);
                        if (q.y > top)
                        {
                            retval.clipped |= _hidden::ClipQTop(p, q, top);
                        }
                        retval.visible = true;
                    }
                    break;
                    
                case 0x4A:
                    retval.clipped = _hidden::ClipPBottom(p, q, bottom);
                    if (p.x > right)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipQRight(p, q, right);
                        if (q.y > top)
                        {
                            retval.clipped |= _hidden::ClipQTop(p, q, top);
                        }
                        retval.visible = true;
                    }
                    break;
                    
                    //---------------------------------------------
                    
                case 0x50:
                    retval.clipped = _hidden::ClipPLeft(p, q, left);
                    if (p.y < bottom)
                    {
                        retval.clipped |= _hidden::ClipPBottom(p, q, bottom);
                    }
                    retval.visible = true;
                    break;
                    
                case 0x51:
                    break;
                    
                case 0x52:
                    retval.clipped = _hidden::ClipQRight(p, q, right);
                    if (q.y < bottom)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipPBottom(p, q, bottom);
                        if (p.x < left)
                        {
                            retval.clipped |= _hidden::ClipPLeft(p, q, left);
                        }
                        retval.visible = true;
                    }
                    break;
                    
                case 0x54:
                    break;
                    
                case 0x55:
                    break;
                    
                case 0x56:
                    break;
                    
                case 0x58:
                    retval.clipped = _hidden::ClipQTop(p, q, top);
                    if (q.x < left)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipPBottom(p, q, bottom);
                        if (p.x < left)
                        {
                            retval.clipped |= _hidden::ClipPLeft(p, q, left);
                        }
                        retval.visible = true;
                    }
                    
                case 0x59:
                    break;
                    
                case 0x5A:
                    retval.clipped = _hidden::ClipPLeft(p, q, left);
                    if (p.y < left)
                    {
                    }
                    else {
                        retval.clipped |= _hidden::ClipQRight(p, q, right);
                        if (q.y < bottom)
                        {
                        }
                        else
                        {
                            if (p.y < bottom)
                            {
                                retval.clipped |= _hidden::ClipPBottom(p, q, bottom);
                            }
                            if (q.y > top)
                            {
                                retval.clipped |= _hidden::ClipQTop(p, q, top);
                            }
                            retval.visible = true;
                        }
                    }
                    break;
                    
                    //---------------------------------------------
                    
                case 0x60:
                    retval.clipped = _hidden::ClipPRight(p, q, right);
                    if (p.y < bottom)
                    {
                        retval.clipped |= _hidden::ClipPBottom(p, q, bottom);
                    }
                    retval.visible = true;
                    break;
                    
                case 0x61:
                    retval.clipped = _hidden::ClipQLeft(p, q, left);
                    if (q.y < bottom)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipPBottom(p, q, bottom);
                        if (p.x > right)
                        {
                            retval.clipped |= _hidden::ClipPRight(p, q, right);
                        }
                        retval.visible = true;
                    }
                    break;
                    
                case 0x62:
                    break;
                    
                case 0x64:
                    break;
                    
                case 0x65:
                    break;
                    
                case 0x66:
                    break;
                    
                case 0x68:
                    retval.clipped = _hidden::ClipQTop(p, q, top);
                    if (q.x > right)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipPRight(p, q, right);
                        if (p.y < bottom)
                        {
                            retval.clipped |= _hidden::ClipPBottom(p, q, bottom);
                        }
                        retval.visible = true;
                    }
                    break;
                    
                case 0x69:
                    retval.clipped = _hidden::ClipQLeft(p, q, left);
                    if (q.y < bottom)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipPRight(p, q, right);
                        if (p.y > top)
                        {
                        }
                        else
                        {
                            if (q.y > top)
                            {
                                retval.clipped |= _hidden::ClipQTop(p, q, top);
                            }
                            if (p.y < bottom)
                            {
                                retval.clipped |= _hidden::ClipPBottom(p, q, bottom);
                            }
                            retval.visible = true;
                        }
                    }
                    break;
                    
                case 0x6A:
                    break;
                    
                    //---------------------------------------------
                    
                case 0x80:
                    retval.clipped = _hidden::ClipPTop(p, q, top);
                    retval.visible = true;
                    break;
                    
                case 0x81:
                    retval.clipped = _hidden::ClipPTop(p, q, top);
                    if (p.x < left)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipQLeft(p, q, left);
                        retval.visible = true;
                    }
                    break;
                    
                case 0x82:
                    retval.clipped = _hidden::ClipPTop(p, q, top);
                    if (p.x > right)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipQRight(p, q, right);
                        retval.visible = true;
                    }
                    break;
                    
                case 0x84:
                    retval.clipped = _hidden::ClipPTop(p, q, top);
                    retval.clipped |= _hidden::ClipQBottom(p, q, bottom);
                    retval.visible = true;
                    break;
                    
                case 0x85:
                    retval.clipped = _hidden::ClipPTop(p, q, top);
                    if (p.x < left)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipQLeft(p, q, left);
                        if (q.y < bottom)
                        {
                            retval.clipped |= _hidden::ClipQBottom(p, q, bottom);
                        }
                        retval.visible = true;
                    }
                    break;
                    
                case 0x86:
                    retval.clipped = _hidden::ClipPTop(p, q, top);
                    if (p.x > right)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipQRight(p, q, right);
                        if (q.y < bottom)
                        {
                            retval.clipped |= _hidden::ClipQBottom(p, q, bottom);
                        }
                        retval.visible = true;
                    }
                    break;
                    
                case 0x88:
                    break;
                    
                case 0x89:
                    break;
                    
                case 0x8A:
                    break;
                    
                    //---------------------------------------------
                    
                case 0x90:
                    retval.clipped = _hidden::ClipPLeft(p, q, left);
                    if (p.y > top)
                    {
                        retval.clipped |= _hidden::ClipPTop(p, q, top);
                    }
                    retval.visible = true;
                    break;
                    
                case 0x91:
                    break;
                    
                case 0x92:
                    retval.clipped = _hidden::ClipQRight(p, q, right);
                    if (q.y > top)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipPTop(p, q, top);
                        if (p.x < left)
                        {
                            retval.clipped |= _hidden::ClipPLeft(p, q, left);
                        }
                        retval.visible = true;
                    }
                    break;
                    
                case 0x94:
                    retval.clipped = _hidden::ClipQBottom(p, q, bottom);
                    if (q.x < left)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipPLeft(p, q, left);
                        if (p.y > top)
                        {
                            retval.clipped |= _hidden::ClipPTop(p, q, top);
                        }
                        retval.visible = true;
                    }
                    break;
                    
                case 0x95:
                    break;
                    
                case 0x96:
                    retval.clipped = _hidden::ClipPLeft(p, q, left);
                    if (p.y < bottom)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipQRight(p, q, right);
                        if (q.y > top)
                        {
                        }
                        else
                        {
                            if (p.y > top)
                            {
                                retval.clipped |= _hidden::ClipPTop(p, q, top);
                            }
                            if (q.y < bottom)
                            {
                                retval.clipped |= _hidden::ClipQBottom(p, q, bottom);
                            }
                            retval.visible = true;
                        }
                    }
                    break;
                    
                case 0x98:
                    break;
                    
                case 0x99:
                    break;
                    
                case 0x9A:
                    break;
                    
                    //---------------------------------------------
                    
                case 0xA0:
                    retval.clipped = _hidden::ClipPRight(p, q, right);
                    if (p.y > top)
                    {
                        retval.clipped |= _hidden::ClipPTop(p, q, top);
                    }
                    retval.visible = true;
                    break;
                    
                case 0xA1:
                    retval.clipped = _hidden::ClipQLeft(p, q, left);
                    if (q.y > top)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipPTop(p, q, top);
                        if (p.x > right)
                        {
                            retval.clipped |= _hidden::ClipPRight(p, q, right);
                        }
                        retval.visible = true;
                    }
                    break;
                    
                case 0xA2:
                    break;
                    
                case 0xA4:
                    retval.clipped = _hidden::ClipQBottom(p, q, bottom);
                    if (q.x > right)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipPRight(p, q, right);
                        if (p.y > top)
                        {
                            retval.clipped |= _hidden::ClipPTop(p, q, top);
                        }
                        retval.visible = true;
                    }
                    break;
                    
                case 0xA5:
                    retval.clipped = _hidden::ClipQLeft(p, q, left);
                    if (q.y > top)
                    {
                    }
                    else
                    {
                        retval.clipped |= _hidden::ClipPRight(p, q, right);
                        if (p.y < bottom)
                        {
                        }
                        else
                        {
                            if (q.y < bottom)
                            {
                                retval.clipped |= _hidden::ClipQBottom(p, q, bottom);
                            }
                            if (p.y > top)
                            {
                                retval.clipped |= _hidden::ClipPTop(p, q, top);
                            }
                            retval.visible = true;
                        }
                    }
                    break;
                    
                case 0xA6:
                    break;
                    
                case 0xA8:
                    break;
                    
                case 0xA9:
                    break;
                    
                case 0xAA:
                    break;
                    
                default:
                    NP_ASSERT(false, "We arrived at an undefined case("+to_str((i32)line_code)+") in ClipLine.");
                    break;
            };
            
            return retval;
        }
        
        /**
         clips given line pq given top and right vaues, assuming bottom and left values are zero
         */
        static inline ClipLineReturn ClipLine(fltPoint &p, fltPoint &q,
                                       ui16 top, ui16 right)
        {
            return ClipLine(p, q, top, right, 0, 0);
        }
    }
}

#endif /* NP_ENGINE_CLIP_LINE_HPP */
