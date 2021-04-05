//
//  DmsImage.hpp
//  Project Space
//
//  Created by Nathan Phipps on 5/17/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_DMS_IMAGE_HPP
#define NP_ENGINE_DMS_IMAGE_HPP

#include "Serialization/Serialization.hpp"
#include "Primitive/Primitive.hpp"
#include "Math/Math.hpp"

#include "DrawableImage.hpp"
#include "DmsLineSegment.hpp"

//TODO: resolve all NP_ASSERTs here

namespace np
{
    namespace core
    {
        /**
         DmsImage is to be treated as a 2D array of ui16 elements and provides shape extraction via the dual marching squares algorithm
         DmsImage inner array is created on construction
         */
        template <typename T, ui16 SIZE>
        class DmsImage : public DrawableImage<T, SIZE>
        {
        private:
            
            using self_type = DmsImage<T, SIZE>;
            using base = DrawableImage<T, SIZE>;
            
            constexpr static chr BaseDrawableImageDir[] = "base_drawable_image_dir";
            
            /**
             gets the filename based on T
             */
            inline static str GetFilename()
            {
                str filename = "";
                
                if constexpr (typetraits::IsSame<T, ui8> ||
                              typetraits::IsSame<T, ui16> ||
                              typetraits::IsSame<T, ui32> ||
                              typetraits::IsSame<T, ui64>)
                {
                    filename = "ui" + to_str(8 * sizeof(T)) + "DmsImage" + to_str(SIZE) + ".json";
                }
                else if constexpr (typetraits::IsSame<T, i8> ||
                                   typetraits::IsSame<T, i16> ||
                                   typetraits::IsSame<T, i32> ||
                                   typetraits::IsSame<T, i64>)
                {
                    filename = "i" + to_str(8 * sizeof(T)) + "DmsImage" + to_str(SIZE) + ".json";
                }
                NP_ELIF_IS_SAME(T, flt)
                {
                    filename = "fltDmsImage" + to_str(SIZE) + ".json";
                }
                NP_ELIF_IS_SAME(T, dbl)
                {
                    filename = "dblDmsImage" + to_str(SIZE) + ".json";
                }
                NP_ELIF_IS_SAME(T, chr)
                {
                    filename = "chrDmsImage" + to_str(SIZE) + ".json";
                }
                NP_ELIF_IS_SAME(T, uchr)
                {
                    filename = "uchrDmsImage" + to_str(SIZE) + ".json";
                }
                NP_ELIF_IS_SAME(T, bl)
                {
                    filename = "blDmsImage" + to_str(SIZE) + ".json";
                }
                NP_ELIF_IS_BASE_OF(T, serialization::Serializable)
                {
                    filename = "DmsImage.json";
                }
                
                return filename;
            }
            
            /**
             checks all eight neighbors to see if given point is an isovalue edge point
             */
            inline bl IsIsovalueEdgePoint(const math::ui16Point point, const flt isohreshold)
            {
                return IsIsovalueEdgePoint(point.x, point.y, isohreshold);
            }
            
            /**
             checks all eight neighbors to see if given (x, y) is an isovalue edge point
             */
            inline bl IsIsovalueEdgePoint(const ui16 x, const ui16 y, const flt isohreshold)
            {
                bl reachhreshold = GetIsovalue(x, y) >= isohreshold;
                bl result = false;
                
                result |= x == 0;
                result |= x == base::GetWidth() - 1;
                result |= y == 0;
                result |= y == base::GetHeight() - 1;
                
                if (!result && reachhreshold)
                {
                    if (x - 1 >= 0)
                    {
                        result |= GetIsovalue(x - 1, y) < isohreshold;
                        
                        if (y - 1 >= 0)
                        {
                            result |= GetIsovalue(x - 1, y - 1) < isohreshold;
                        }
                        
                        if (y + 1 < base::GetHeight())
                        {
                            result |= GetIsovalue(x - 1, y + 1) < isohreshold;
                        }
                    }
                    
                    if (x + 1 < base::GetWidth())
                    {
                        result |= GetIsovalue(x + 1, y) < isohreshold;
                        
                        if (y - 1 >= 0)
                        {
                            result |= GetIsovalue(x + 1, y - 1) < isohreshold;
                        }
                        
                        if (y + 1 < base::GetHeight())
                        {
                            result |= GetIsovalue(x + 1, y + 1) < isohreshold;
                        }
                    }
                    
                    if (y - 1 >= 0)
                    {
                        result |= GetIsovalue(x, y - 1) < isohreshold;
                    }
                    
                    if (y + 1 < base::GetHeight())
                    {
                        result |= GetIsovalue(x, y + 1) < isohreshold;
                    }
                }
                
                return result && reachhreshold;
            }
            
            /**
             gets line segments given iso threshold around given point and stores them into given line segement uset
             */
            inline void GetLineSegmentsAroundPoint(DmsLineSegmentUset& segments,
                                                   const math::ui16Point point,
                                                   const flt isohreshold)
            {
                /* point names in reference of given point
                 upper_left     upper       upper_right
                 left           point       right
                 lower_left     lower       lower_right
                 */
                
                //TODO:  consider origin
                if (point.x == 0 || point.x == base::GetWidth() - 1 ||
                    point.y == 0 || point.y == base::GetHeight() - 1)
                {
                    //get edge line segments
                    
                    GetBorderLineSegmentsAroundPoint(segments, point, isohreshold);
                }
                
                math::ui16Point lower_left = point;
                lower_left.x--;
                lower_left.y--;
                math::ui16Point upper_right = point;
                upper_right.x++;
                upper_right.y++;
                math::ui16Point left = point;
                left.x--;
                math::ui16Point right = point;
                right.x++;
                math::ui16Point lower = point;
                lower.y--;
                math::ui16Point upper = point;
                upper.y++;
                
                //ensure no wrap around
                if (left.x < point.x && upper.y > point.y)
                {
                    //ensure points are within image
                    //left is in image since no wrap around
                    if (upper.y < base::GetHeight())
                    {
                        GetLineSegment(segments, point, isohreshold, left, upper);
                    }
                }
                
                //ensure no wrap around
                if (lower_left.x < point.x && lower_left.y < point.y)
                {
                    //ensure points are within image
                    //lower_left is in image since no wrap around
                    GetLineSegment(segments, point, isohreshold, lower_left, point);
                }
                
                //ensure no wrap around
                if (upper_right.x > point.x && upper_right.y > point.y)
                {
                    //ensure points are within image
                    if (upper_right.x < base::GetWidth() && upper_right.y < base::GetHeight())
                    {
                        GetLineSegment(segments, point, isohreshold, point, upper_right);
                    }
                }
                
                //ensure no wrap around
                if (lower.y < point.y && right.x > point.x)
                {
                    //ensure points are within image
                    //lower is in image since no wrap around
                    if (right.x < base::GetWidth())
                    {
                        GetLineSegment(segments, point, isohreshold, lower, right);
                    }
                }
            }
            
            /**
             gets isovalue of given point along the outside border of this image
             */
            inline flt GetBorderIsovalue(const math::fltPoint point)
            {
                flt bad_iso = -FLT_MAX;
                flt isovalue = bad_iso;
                
                if (point.x >= 0 && point.x < base::GetWidth())
                {
                    if (point.y >= 0 && point.y < base::GetHeight())
                    {
                        //point.x and point.y are in bounds, so we floor to get index of our relative pixel
                        math::ui16Point point_as_ui16
                        {
                            .x = (ui16)floor(point.x),
                            .y = (ui16)floor(point.y)
                        };
                        
                        isovalue = GetIsovalue(point_as_ui16);
                    }
                    else
                    {
                        isovalue = GetBorderIsovalue(*this, point);
                    }
                }
                else
                {
                    isovalue = GetBorderIsovalue(*this, point);
                }
                
                NP_ASSERT(isovalue != bad_iso, "we need to guarantee we return valid isovalue");
                
                return isovalue;
            }
            
            /**
             gets the line segments given iso threshold around given point along the outside border of this image and stores them into give line segment uset
             */
            inline void GetBorderLineSegmentsAroundPoint(DmsLineSegmentUset& segments,
                                                         const math::ui16Point point,
                                                         const flt isohreshold)
            {
                /* point names in reference of given point
                 upper_left     upper       upper_right
                 left           fPoint     right
                 lower_left     lower       lower_right
                 */
                
                math::fltPoint fPoint
                {
                    .x = (flt)point.x,
                    .y = (flt)point.y
                };
                
                math::fltPoint lower_left = fPoint;
                lower_left.x--;
                lower_left.y--;
                math::fltPoint upper_right = fPoint;
                upper_right.x++;
                upper_right.y++;
                math::fltPoint left = fPoint;
                left.x--;
                math::fltPoint right = fPoint;
                right.x++;
                math::fltPoint lower = fPoint;
                lower.y--;
                math::fltPoint upper = fPoint;
                upper.y++;
                
                if (point.x == 0)
                {
                    GetBorderLineSegment(segments, point, isohreshold, lower_left, fPoint);
                    GetBorderLineSegment(segments, point, isohreshold, left, upper);
                    
                    if (point.y == 0)
                    {
                        GetBorderLineSegment(segments, point, isohreshold, lower, right);
                    }
                    else if (point.y == base::GetHeight() - 1)
                    {
                        GetBorderLineSegment(segments, point, isohreshold, fPoint, upper_right);
                    }
                }
                else if (point.x == base::GetWidth() - 1)
                {
                    GetBorderLineSegment(segments, point, isohreshold, lower, right);
                    GetBorderLineSegment(segments, point, isohreshold, fPoint, upper_right);
                    
                    if (point.y == 0)
                    {
                        GetBorderLineSegment(segments, point, isohreshold, lower_left, fPoint);
                    }
                    else if (point.y == base::GetHeight() - 1)
                    {
                        GetBorderLineSegment(segments, point, isohreshold, left, upper);
                    }
                }
                else //should we just take the inner ifs and bring those out??
                {
                    if (point.y == 0)
                    {
                        GetBorderLineSegment(segments, point, isohreshold, lower_left, fPoint);
                        GetBorderLineSegment(segments, point, isohreshold, lower, right);
                    }
                    else if (point.y == base::GetHeight() - 1)
                    {
                        GetBorderLineSegment(segments, point, isohreshold, fPoint, upper_right);
                        GetBorderLineSegment(segments, point, isohreshold, left, upper);
                    }
                    else
                    {
                        //we received incorrect point
                        NP_ASSERT(false, "we received an incorrect point in DmsImage::GetBorderLineSegmentsAroundPoint");
                    }
                }
            }
            
            /**
             gets the line segments given iso threshold around given point along the outside border of this image and stores them into give line segment uset
             this method is meant to be used by GetBorderLineSegmentsAroundPoint
             */
            inline void GetBorderLineSegment(DmsLineSegmentUset& segments,
                                             const math::ui16Point point,
                                             const flt isohreshold,
                                             const math::fltPoint llPoint,
                                             const math::fltPoint urPoint)
            {
                //TODO:  finish the switch statement
                
                math::fltPoint fPoint
                {
                    .x = (flt)point.x,
                    .y = (flt)point.y
                };
                math::fltPoint ulPoint{.x=llPoint.x, .y=urPoint.y};
                math::fltPoint lrPoint{.x=urPoint.x, .y=llPoint.y};
                
                bl left_edge = fPoint.x == 0;
                bl right_edge = fPoint.x == base::GetWidth() - 1;
                bl upper_edge = fPoint.y == base::GetHeight() - 1;
                bl lower_edge = fPoint.y == 0;
                
                flt ll_iso = GetBorderIsovalue(llPoint);
                flt lr_iso = GetBorderIsovalue(lrPoint);
                flt ul_iso = GetBorderIsovalue(ulPoint);
                flt ur_iso = GetBorderIsovalue(urPoint);
                
                ui16 segmentType = 0;
                
                segmentType += ll_iso > isohreshold ? 8 : 0;
                segmentType += ul_iso > isohreshold ? 4 : 0;
                segmentType += ur_iso > isohreshold ? 2 : 0;
                segmentType += lr_iso > isohreshold ? 1 : 0;
                
                math::fltPoint begin;
                math::fltPoint end;
                
                //TODO: we should be guaranteed an edge?
                bl edge_found = true; //maybe false for case 15?
                math::fltPoint edge_begin;
                math::fltPoint edge_end;
                
                bl corner_found = false;
                math::fltPoint corner_begin;
                math::fltPoint corner_end;
                
                bl type_found = true;
                
                switch (segmentType)
                {
                    case 0:
                        type_found = false;
                        break;
                    case 1:
                        if (fPoint == lrPoint)
                        /*TODO: all these ifs should always be true?
                         A: Yes, only because we're guaranteeing
                         associatedPoint was collected from flood fill region. If we did a stride-2
                         technique, we would need to figure out which point in the 2x2 sample will
                         be the associatedPoint, so we'd need to handle that later
                         */
                        {
                            //as lower or left border, we only have to care if the begin point needs to be culled
                            //as upper or right border, we only have to care if the end point needs to be culled
                            return;
                            
                            flt begin_t = (isohreshold - ll_iso) / (lr_iso - ll_iso);
                            flt end_t = (isohreshold - ur_iso) / (lr_iso - ur_iso);
                            
                            begin = math::lerp(llPoint, lrPoint, begin_t);
                            end = math::lerp(urPoint, lrPoint, end_t);
                            
                            //TODO: handle edge_begin
                        }
                        break;
                    case 2:
                        if (fPoint == urPoint)
                        {
                            return;
                            
                            flt begin_t = (isohreshold - ul_iso) / (ur_iso - ul_iso);
                            flt end_t = (isohreshold - lr_iso) / (ur_iso - lr_iso);
                            
                            begin = math::lerp(ulPoint, urPoint, begin_t);
                            end = math::lerp(lrPoint, urPoint, end_t);
                            
                            //TODO: handle edge_begin
                        }
                        break;
                    case 3:
                        if (fPoint == lrPoint ||
                            fPoint == urPoint)
                        {
                            //return;
                            
                            flt begin_t = (isohreshold - ll_iso) / (lr_iso - ll_iso);
                            flt end_t = (isohreshold - ul_iso) / (ur_iso - ul_iso);
                            
                            begin = math::lerp(llPoint, lrPoint, begin_t);
                            end = math::lerp(ulPoint, urPoint, end_t);
                            
                            //TODO: add clip line method
                            begin += 0.5f;
                            end += 0.5f;
                            math::ClipLineReturn line = math::ClipLine(begin, end, base::GetHeight(), base::GetWidth());
                            if (line.visible)
                            {
                                roundTo32nd(begin);
                                roundTo32nd(end);
                                if (begin != end)
                                {
                                    segments.insert(DmsLineSegment(begin, end, point));
                                }
                            }
                            
                            //edge line
                            if (end < begin)
                            {
                                math::fltPoint temp = begin;
                                begin = end;
                                end = temp;
                            }
                            NP_ASSERT(begin < end, "edge line error");
                            
                            edge_begin = urPoint;
                            edge_begin.x += 0.5f;
                            edge_end = edge_begin;
                            edge_end.x = begin.x;
                            
                            roundTo32nd(edge_begin);
                            roundTo32nd(edge_end);
                            
                            if (edge_begin != edge_end)
                            {
                                //TODO:  do we need to use this technique every else?
                                if (lower_edge)
                                {
                                    segments.insert(DmsLineSegment(edge_begin, edge_end, point, edge_end));
                                }
                                else if (upper_edge)
                                {
                                    segments.insert(DmsLineSegment(edge_begin, edge_end, point, edge_end));
                                }
                                else
                                {
                                    NP_ASSERT(false, "we should have been on edge");
                                }
                            }
                        }
                        break;
                    case 4:
                        if (fPoint == ulPoint)
                        {
                            return;
                            
                            flt begin_t = (isohreshold - ll_iso) / (ul_iso - ll_iso);
                            flt end_t = (isohreshold - ur_iso) / (ul_iso - ur_iso);
                            
                            begin = math::lerp(llPoint, ulPoint, begin_t);
                            end = math::lerp(urPoint, ulPoint, end_t);
                            
                            //TODO: handle edge_begin
                        }
                        break;
                    case 5:
                        if (fPoint == ulPoint)
                        {
                            return;
                            
                            flt begin_t = (isohreshold - ll_iso) / (ul_iso - ll_iso);
                            flt end_t = (isohreshold - ur_iso) / (ul_iso - ur_iso);
                            
                            begin = math::lerp(llPoint, ulPoint, begin_t);
                            end = math::lerp(urPoint, ulPoint, end_t);
                            
                            //TODO: handle edge_begin
                        }
                        else if (fPoint == lrPoint)
                        {
                            return;
                            
                            flt begin_t = (isohreshold - ll_iso) / (lr_iso - ll_iso);
                            flt end_t = (isohreshold - ur_iso) / (lr_iso - ur_iso);
                            
                            begin = math::lerp(llPoint, lrPoint, begin_t);
                            end = math::lerp(urPoint, lrPoint, end_t);
                            
                            //TODO: handle edge_begin
                        }
                        break;
                    case 6:
                        if (fPoint == ulPoint ||
                            fPoint == urPoint)
                        {
                            //return;
                            
                            flt begin_t = (isohreshold - ll_iso) / (ul_iso - ll_iso);
                            flt end_t = (isohreshold - lr_iso) / (ur_iso - lr_iso);
                            
                            begin = math::lerp(llPoint, ulPoint, begin_t);
                            end = math::lerp(lrPoint, urPoint, end_t);
                            
                            //TODO: add clip line method
                            begin += 0.5f;
                            end += 0.5f;
                            math::ClipLineReturn line = math::ClipLine(begin, end, base::GetHeight(), base::GetWidth());
                            if (line.visible)
                            {
                                roundTo32nd(begin);
                                roundTo32nd(end);
                                if (begin != end)
                                {
                                    segments.insert(DmsLineSegment(begin, end, point));
                                }
                            }
                            
                            //edge line
                            if (end < begin)
                            {
                                math::fltPoint temp = begin;
                                begin = end;
                                end = temp;
                            }
                            NP_ASSERT(begin < end, "our begin and end are reversed");
                            
                            edge_begin = urPoint;
                            edge_begin.y += 0.5f;
                            edge_end = edge_begin;
                            edge_end.y = begin.y;
                            
                            roundTo32nd(edge_begin);
                            roundTo32nd(edge_end);
                            
                            if (edge_begin != edge_end)
                            {
                                if (left_edge)
                                {
                                    segments.insert(DmsLineSegment(edge_begin, edge_end, point, edge_end));
                                }
                                else if (right_edge)
                                {
                                    segments.insert(DmsLineSegment(edge_begin, edge_end, point, edge_end));
                                }
                                else
                                {
                                    NP_ASSERT(false, "we should have been on edge");
                                }
                            }
                        }
                        break;
                    case 7:
                        if (fPoint == ulPoint ||
                            fPoint == urPoint ||
                            fPoint == lrPoint)
                        {
                            return;
                            
                            flt begin_t = (isohreshold - ll_iso) / (ul_iso - ll_iso);
                            flt end_t = (isohreshold - ll_iso) / (lr_iso - ll_iso);
                            
                            begin = math::lerp(llPoint, ulPoint, begin_t);
                            end = math::lerp(llPoint, lrPoint, end_t);
                            
                            //TODO: handle edge_begin
                        }
                        break;
                    case 8:
                        if (fPoint == llPoint)
                        {
                            return;
                            
                            flt begin_t = (isohreshold - ul_iso) / (ll_iso - ul_iso);
                            flt end_t = (isohreshold - lr_iso) / (ll_iso - lr_iso);
                            
                            begin = math::lerp(ulPoint, llPoint, begin_t);
                            end = math::lerp(lrPoint, llPoint, end_t);
                        }
                        break;
                    case 9:
                        if (fPoint == llPoint ||
                            fPoint == lrPoint)
                        {
                            //return;
                            
                            flt begin_t = (isohreshold - ul_iso) / (ll_iso - ul_iso);
                            flt end_t = (isohreshold - ur_iso) / (lr_iso - ur_iso);
                            
                            begin = math::lerp(ulPoint, llPoint, begin_t);
                            end = math::lerp(urPoint, lrPoint, end_t);
                            
                            //TODO: add clip line method
                            begin += 0.5f;
                            end += 0.5f;
                            math::ClipLineReturn line = math::ClipLine(begin, end, base::GetHeight(), base::GetWidth());
                            if (line.visible)
                            {
                                roundTo32nd(begin);
                                roundTo32nd(end);
                                if (begin != end)
                                {
                                    segments.insert(DmsLineSegment(begin, end, point));
                                }
                            }
                            
                            //edge line
                            if (end < begin)
                            {
                                math::fltPoint temp = begin;
                                begin = end;
                                end = temp;
                            }
                            NP_ASSERT(begin < end, "our points are reversed");
                            
                            edge_begin = lrPoint;
                            edge_begin.y += 0.5f;
                            edge_end = edge_begin;
                            edge_end.y = end.y;
                            
                            roundTo32nd(edge_begin);
                            roundTo32nd(edge_end);
                            
                            if (edge_begin != edge_end)
                            {
                                if (left_edge)
                                {
                                    segments.insert(DmsLineSegment(edge_begin, edge_end, point, edge_end));
                                }
                                else if (right_edge)
                                {
                                    segments.insert(DmsLineSegment(edge_begin, edge_end, point, edge_end));
                                }
                                else
                                {
                                    NP_ASSERT(false, "points should have been an edge");
                                }
                            }
                        }
                        break;
                    case 10:
                        if (fPoint == llPoint)
                        {
                            return;
                            
                            flt begin_t = (isohreshold - ul_iso) / (ll_iso - ul_iso);
                            flt end_t = (isohreshold - lr_iso) / (ll_iso - lr_iso);
                            
                            begin = math::lerp(ulPoint, llPoint, begin_t);
                            end = math::lerp(lrPoint, llPoint, end_t);
                            
                            //TODO: handle edge_begin
                        }
                        else if (fPoint == urPoint)
                        {
                            return;
                            
                            flt begin_t = (isohreshold - ul_iso) / (ur_iso - ul_iso);
                            flt end_t = (isohreshold - lr_iso) / (ur_iso - lr_iso);
                            
                            begin = math::lerp(ulPoint, urPoint, begin_t);
                            end = math::lerp(lrPoint, urPoint, end_t);
                            
                            //TODO: handle edge_begin
                        }
                        break;
                    case 11:
                        if (fPoint == llPoint ||
                            fPoint == urPoint ||
                            fPoint == lrPoint)
                        {
                            return;
                            
                            flt begin_t = (isohreshold - ul_iso) / (ll_iso - ul_iso);
                            flt end_t = (isohreshold - ul_iso) / (ur_iso - ul_iso);
                            
                            begin = math::lerp(ulPoint, llPoint, begin_t);
                            end = math::lerp(ulPoint, urPoint, end_t);
                            
                            //TODO: handle edge_begin
                        }
                        break;
                    case 12:
                        if (fPoint == llPoint ||
                            fPoint == ulPoint)
                        {
                            //return;
                            
                            flt begin_t = (isohreshold - lr_iso) / (ll_iso - lr_iso);
                            flt end_t = (isohreshold - ur_iso) / (ul_iso - ur_iso);
                            
                            begin = math::lerp(lrPoint, llPoint, begin_t);
                            end = math::lerp(urPoint, ulPoint, end_t);
                            
                            //TODO: add clip line method
                            begin += 0.5f;
                            end += 0.5f;
                            math::ClipLineReturn line = math::ClipLine(begin, end, base::GetHeight(), base::GetWidth());
                            if (line.visible)
                            {
                                roundTo32nd(begin);
                                roundTo32nd(end);
                                if (begin != end)
                                {
                                    segments.insert(DmsLineSegment(begin, end, point));
                                }
                            }
                            
                            //edge line
                            if (end < begin)
                            {
                                math::fltPoint temp = begin;
                                begin = end;
                                end = temp;
                            }
                            NP_ASSERT(begin < end, "points are reversed");
                            
                            edge_begin = ulPoint;
                            edge_begin.x += 0.5f;
                            edge_end = edge_begin;
                            edge_end.x = begin.x;
                            
                            roundTo32nd(edge_begin);
                            roundTo32nd(edge_end);
                            
                            if (edge_begin != edge_end)
                            {
                                if (lower_edge)
                                {
                                    segments.insert(DmsLineSegment(edge_begin, edge_end,
                                                                                point, edge_end));
                                }
                                else if (upper_edge)
                                {
                                    segments.insert(DmsLineSegment(edge_begin, edge_end,
                                                                                point, edge_end));
                                }
                                else
                                {
                                    NP_ASSERT(false, "our points should have been edge");
                                }
                            }
                        }
                        break;
                    case 13:
                        if (fPoint == llPoint ||
                            fPoint == ulPoint ||
                            fPoint == lrPoint)
                        {
                            return;
                            
                            flt begin_t = (isohreshold - ur_iso) / (ul_iso - ur_iso);
                            flt end_t = (isohreshold - ur_iso) / (lr_iso - ur_iso);
                            
                            begin = math::lerp(urPoint, ulPoint, begin_t);
                            end = math::lerp(urPoint, lrPoint, end_t);
                            
                            //TODO: handle edge_begin
                        }
                        break;
                    case 14:
                        if (fPoint == llPoint ||
                            fPoint == ulPoint ||
                            fPoint == urPoint)
                        {
                            return;
                            
                            flt begin_t = (isohreshold - lr_iso) / (ll_iso - lr_iso);
                            flt end_t = (isohreshold - lr_iso) / (ur_iso - lr_iso);
                            
                            begin = math::lerp(lrPoint, llPoint, begin_t);
                            end = math::lerp(lrPoint, urPoint, end_t);
                            
                            //TODO: handle edge_begin
                        }
                        break;
                    case 15:
                    {
                        //return;
                        
                        //begin = fPoint;
                        //end = fPoint;
                        //begin.x -= 0.25f;
                        //begin.y -= 0.25f;
                        //end.x += 0.25f;
                        //end.y += 0.25f;
                        //begin += 0.5f;
                        //end += 0.5f;
                        //roundTo32nd(begin);
                        //roundTo32nd(end);
                        //
                        //if (begin != end)
                        //{
                        //    segments.insert(DmsLineSegment(begin, end, point));
                        //}
                        
                        if (left_edge)
                        {
                            if (lower_edge)
                            {
                                corner_found = true;
                                edge_begin = fPoint;
                                edge_end = fPoint;
                                edge_end.y += 0.5f;
                                
                                corner_begin = fPoint;
                                corner_end = fPoint;
                                corner_end.x += 0.5f;
                            }
                            else if (upper_edge)
                            {
                                corner_found = true;
                                edge_begin = fPoint;
                                edge_end = fPoint;
                                edge_begin.y += 0.5f;
                                edge_end.y += 1.0f;
                                
                                corner_begin = fPoint;
                                corner_end = fPoint;
                                corner_begin.y += 1.0f;
                                corner_end.x += 0.5f;
                                corner_end.y += 1.0f;
                            }
                            else
                            {
                                edge_begin = lrPoint;
                                edge_end = urPoint;
                                edge_begin.y += 0.5f;
                                edge_end.y += 0.5f;
                            }
                        }
                        else if (right_edge)
                        {
                            if (lower_edge)
                            {
                                corner_found = true;
                                edge_begin = fPoint;
                                edge_end = fPoint;
                                edge_begin.x += 1.0f;
                                edge_end.x += 1.0f;
                                edge_end.y += 0.5f;
                                
                                corner_begin = fPoint;
                                corner_end = fPoint;
                                corner_begin.x += 0.5f;
                                corner_end.x += 1.0f;
                                
                            }
                            else if (upper_edge)
                            {
                                corner_found = true;
                                edge_begin = fPoint;
                                edge_end = fPoint;
                                edge_begin.x += 1.0f;
                                edge_begin.y += 0.5f;
                                edge_end.x += 1.0f;
                                edge_end.y += 1.0f;
                                
                                corner_begin = fPoint;
                                corner_end = fPoint;
                                corner_begin.x += 0.5f;
                                corner_begin.y += 1.0f;
                                corner_end.x += 1.0f;
                                corner_end.y += 1.0f;
                            }
                            else
                            {
                                edge_begin = lrPoint;
                                edge_end = urPoint;
                                edge_begin.y += 0.5f;
                                edge_end.y += 0.5f;
                            }
                        }
                        else
                        {
                            if (lower_edge)
                            {
                                edge_begin = ulPoint;
                                edge_end = urPoint;
                                edge_begin.x += 0.5f;
                                edge_end.x += 0.5f;
                            }
                            else if (upper_edge)
                            {
                                edge_begin = ulPoint;
                                edge_end = urPoint;
                                edge_begin.x += 0.5f;
                                edge_end.x += 0.5f;
                            }
                        }
                        
                        //TODO:  specifiy midpoint for corner case
                        
                        roundTo32nd(edge_begin);
                        roundTo32nd(edge_end);
                        
                        if (edge_begin != edge_end)
                        {
                            segments.insert(DmsLineSegment(edge_begin, edge_end, point));
                        }
                        
                        roundTo32nd(corner_begin);
                        roundTo32nd(corner_end);
                        
                        if (corner_begin != corner_end)
                        {
                            if (left_edge)
                            {
                                segments.insert(DmsLineSegment(corner_begin, corner_end,
                                                                            point, corner_begin));
                            }
                            else //right_edge
                            {
                                segments.insert(DmsLineSegment(corner_begin, corner_end,
                                                                            point, corner_end));
                            }
                        }
                        
                        break;
                    }
                }
            }
            
            /**
             gets line segments given iso threshold around given point and stores them into given line segement uset
             this method is meant to be used by GetLineSegmentsAroundPoint
             */
            inline void GetLineSegment(DmsLineSegmentUset& segments,
                                       const math::ui16Point point,
                                       const flt isohreshold,
                                       const math::ui16Point llPoint,
                                       const math::ui16Point urPoint)
            {
                math::ui16Point ulPoint{.x=llPoint.x, .y=urPoint.y};
                math::ui16Point lrPoint{.x=urPoint.x, .y=llPoint.y};
                
                flt ll_iso = GetIsovalue(llPoint);
                flt lr_iso = GetIsovalue(lrPoint);
                flt ul_iso = GetIsovalue(ulPoint);
                flt ur_iso = GetIsovalue(urPoint);
                
                ui16 segmentType = 0;
                
                segmentType += ll_iso > isohreshold ? 8 : 0;
                segmentType += ul_iso > isohreshold ? 4 : 0;
                segmentType += ur_iso > isohreshold ? 2 : 0;
                segmentType += lr_iso > isohreshold ? 1 : 0;
                
                math::fltPoint begin;
                math::fltPoint end;
                
                bl type_found = true;
                
                switch (segmentType)
                {
                    case 0:
                        type_found = false;
                        break;
                    case 1:
                        if (point == lrPoint)
                        /*TODO: all these ifs should always be true?
                         A: Yes, only because we're guaranteeing
                         associatedPoint was collected from flood fill region. If we did a stride-2
                         technique, we would need to figure out which point in the 2x2 sample will
                         be the associatedPoint, so we'd need to handle that later
                         */
                        {
                            flt begin_t = (isohreshold - ll_iso) / (lr_iso - ll_iso);
                            flt end_t = (isohreshold - ur_iso) / (lr_iso - ur_iso);
                            
                            begin = math::lerp(llPoint, lrPoint, begin_t);
                            end = math::lerp(urPoint, lrPoint, end_t);
                        }
                        break;
                    case 2:
                        if (point == urPoint)
                        {
                            flt begin_t = (isohreshold - ul_iso) / (ur_iso - ul_iso);
                            flt end_t = (isohreshold - lr_iso) / (ur_iso - lr_iso);
                            
                            begin = math::lerp(ulPoint, urPoint, begin_t);
                            end = math::lerp(lrPoint, urPoint, end_t);
                        }
                        break;
                    case 3:
                        if (point == lrPoint ||
                            point == urPoint)
                        {
                            flt begin_t = (isohreshold - ll_iso) / (lr_iso - ll_iso);
                            flt end_t = (isohreshold - ul_iso) / (ur_iso - ul_iso);
                            
                            begin = math::lerp(llPoint, lrPoint, begin_t);
                            end = math::lerp(ulPoint, urPoint, end_t);
                        }
                        break;
                    case 4:
                        if (point == ulPoint)
                        {
                            flt begin_t = (isohreshold - ll_iso) / (ul_iso - ll_iso);
                            flt end_t = (isohreshold - ur_iso) / (ul_iso - ur_iso);
                            
                            begin = math::lerp(llPoint, ulPoint, begin_t);
                            end = math::lerp(urPoint, ulPoint, end_t);
                        }
                        break;
                    case 5:
                        if (point == ulPoint)
                        {
                            flt begin_t = (isohreshold - ll_iso) / (ul_iso - ll_iso);
                            flt end_t = (isohreshold - ur_iso) / (ul_iso - ur_iso);
                            
                            begin = math::lerp(llPoint, ulPoint, begin_t);
                            end = math::lerp(urPoint, ulPoint, end_t);
                        }
                        else if (point == lrPoint)
                        {
                            flt begin_t = (isohreshold - ll_iso) / (lr_iso - ll_iso);
                            flt end_t = (isohreshold - ur_iso) / (lr_iso - ur_iso);
                            
                            begin = math::lerp(llPoint, lrPoint, begin_t);
                            end = math::lerp(urPoint, lrPoint, end_t);
                        }
                        break;
                    case 6:
                        if (point == ulPoint ||
                            point == urPoint)
                        {
                            flt begin_t = (isohreshold - ll_iso) / (ul_iso - ll_iso);
                            flt end_t = (isohreshold - lr_iso) / (ur_iso - lr_iso);
                            
                            begin = math::lerp(llPoint, ulPoint, begin_t);
                            end = math::lerp(lrPoint, urPoint, end_t);
                        }
                        break;
                    case 7:
                        if (point == ulPoint ||
                            point == urPoint ||
                            point == lrPoint)
                        {
                            flt begin_t = (isohreshold - ll_iso) / (ul_iso - ll_iso);
                            flt end_t = (isohreshold - ll_iso) / (lr_iso - ll_iso);
                            
                            begin = math::lerp(llPoint, ulPoint, begin_t);
                            end = math::lerp(llPoint, lrPoint, end_t);
                        }
                        break;
                    case 8:
                        if (point == llPoint)
                        {
                            flt begin_t = (isohreshold - ul_iso) / (ll_iso - ul_iso);
                            flt end_t = (isohreshold - lr_iso) / (ll_iso - lr_iso);
                            
                            begin = math::lerp(ulPoint, llPoint, begin_t);
                            end = math::lerp(lrPoint, llPoint, end_t);
                        }
                        break;
                    case 9:
                        if (point == llPoint ||
                            point == lrPoint)
                        {
                            flt begin_t = (isohreshold - ul_iso) / (ll_iso - ul_iso);
                            flt end_t = (isohreshold - ur_iso) / (lr_iso - ur_iso);
                            
                            begin = math::lerp(ulPoint, llPoint, begin_t);
                            end = math::lerp(urPoint, lrPoint, end_t);
                        }
                        break;
                    case 10:
                        if (point == llPoint)
                        {
                            flt begin_t = (isohreshold - ul_iso) / (ll_iso - ul_iso);
                            flt end_t = (isohreshold - lr_iso) / (ll_iso - lr_iso);
                            
                            begin = math::lerp(ulPoint, llPoint, begin_t);
                            end = math::lerp(lrPoint, llPoint, end_t);
                        }
                        else if (point == urPoint)
                        {
                            flt begin_t = (isohreshold - ul_iso) / (ur_iso - ul_iso);
                            flt end_t = (isohreshold - lr_iso) / (ur_iso - lr_iso);
                            
                            begin = math::lerp(ulPoint, urPoint, begin_t);
                            end = math::lerp(lrPoint, urPoint, end_t);
                        }
                        break;
                    case 11:
                        if (point == llPoint ||
                            point == urPoint ||
                            point == lrPoint)
                        {
                            flt begin_t = (isohreshold - ul_iso) / (ll_iso - ul_iso);
                            flt end_t = (isohreshold - ul_iso) / (ur_iso - ul_iso);
                            
                            begin = math::lerp(ulPoint, llPoint, begin_t);
                            end = math::lerp(ulPoint, urPoint, end_t);
                        }
                        break;
                    case 12:
                        if (point == llPoint ||
                            point == ulPoint)
                        {
                            flt begin_t = (isohreshold - lr_iso) / (ll_iso - lr_iso);
                            flt end_t = (isohreshold - ur_iso) / (ul_iso - ur_iso);
                            
                            begin = math::lerp(lrPoint, llPoint, begin_t);
                            end = math::lerp(urPoint, ulPoint, end_t);
                        }
                        break;
                    case 13:
                        if (point == llPoint ||
                            point == ulPoint ||
                            point == lrPoint)
                        {
                            flt begin_t = (isohreshold - ur_iso) / (ul_iso - ur_iso);
                            flt end_t = (isohreshold - ur_iso) / (lr_iso - ur_iso);
                            
                            begin = math::lerp(urPoint, ulPoint, begin_t);
                            end = math::lerp(urPoint, lrPoint, end_t);
                        }
                        break;
                    case 14:
                        if (point == llPoint ||
                            point == ulPoint ||
                            point == urPoint)
                        {
                            flt begin_t = (isohreshold - lr_iso) / (ll_iso - lr_iso);
                            flt end_t = (isohreshold - lr_iso) / (ur_iso - lr_iso);
                            
                            begin = math::lerp(lrPoint, llPoint, begin_t);
                            end = math::lerp(lrPoint, urPoint, end_t);
                        }
                        break;
                    case 15:
                        type_found = false;
                        break;
                }
                
                if (type_found)
                {
                    //round begin and end points
                    
                    begin += 0.5f;
                    end += 0.5f;
                    roundTo32nd(begin);
                    roundTo32nd(end);
                    
                    //begin.x = roundTo32nd(begin.x + 0.5f);
                    //begin.y = roundTo32nd(begin.y + 0.5f);
                    //end.x = roundTo32nd(end.x + 0.5f);
                    //end.y = roundTo32nd(end.y + 0.5f);
                    
                    //begin.x = roundTo16th(begin.x);
                    //begin.y = roundTo16th(begin.y);
                    //end.x = roundTo16th(end.x);
                    //end.y = roundTo16th(end.y);
                    
                    if (begin != end)
                    {
                        segments.insert(DmsLineSegment(begin, end, point));
                    }
                }
            }
            
        protected:
            
            /**
             default behavior is to get the isovalue of the edge point closest to given point
             */
            inline virtual flt GetBorderIsovalue(const self_type& reference_image, const math::fltPoint point)
            {
                NP_ASSERT(point.x < 0.f || point.x >= base::GetWidth() ||
                          point.y < 0.f || point.y >= base::GetHeight(),
                          "we should not call this if we don't need to");
                
                //get edge point for border point
                math::ui16Point edgePoint;
                
                if (point.x < 0)
                {
                    edgePoint.x = 0;
                }
                else if (point.x >= base::GetWidth())
                {
                    edgePoint.x = base::GetWidth() - 1;
                }
                else
                {
                    edgePoint.x = (ui16)point.x;
                }
                
                if (point.y < 0)
                {
                    edgePoint.y = 0;
                }
                else if (point.y >= base::GetHeight())
                {
                    edgePoint.y = base::GetHeight() - 1;
                }
                else
                {
                    edgePoint.y = (ui16)point.y;
                }
                
                flt iso = GetIsovalue(edgePoint);
                
                return iso;
            }
            
        public:
            
            /**
             constructor passing to ui16_drawable_image
             */
            DmsImage():
            base()
            {}
            
            /**
             constructor passing to ui16_drawable_image
             */
            DmsImage(const ui16 value):
            base(value)
            {}
            
            /**
             constructor passing to ui16_drawable_image
             */
            DmsImage(const ui16 value, const math::ui16Point origin):
            base(value, origin)
            {}
            
            /**
             constructor passing to ui16_drawable_image
             */
            DmsImage(const math::ui16Point origin):
            base(origin)
            {}
            
            /**
             deconstructor
             */
            virtual ~DmsImage()
            {
                base::Clear();
            }
            
            /**
             gets the dms iso value [0.0 - 1.0] from given point - isovalue  => fraction of maximum value => 128 / 255 = 0.501960784313725
             */
            inline flt GetIsovalue(const math::ui16Point point) const
            {
                return GetIsovalue(point.x, point.y);
            }
            
            /**
             gets the dms iso value [0.0 - 1.0] from given (x, y) - isovalue  => fraction of maximum value => 128 / 255 = 0.501960784313725
             */
            inline virtual flt GetIsovalue(const ui16 x, const ui16 y) const
            {
                //TODO: make GetIsovalue more flexible (defaulting to 0 or 1?) -- make a CalculateIsovalue(value) function pointer?
                
                math::ui16Point origin = base::GetOrigin();
                flt value = (flt)base::GetValue(x + origin.x, y + origin.y);
                flt maxValue = (flt)UINT8_MAX;
                return value / maxValue;
            }
            
            /**
             performs the dual marching squares algorithm
             return object is a vector of shapes with holes
             */
            math::fltPointVector3D* GetPolygonVector3D(const flt isohreshold)
            //container::vector<DmsLineSegmentUset>* GetPolygonVector3D(const flt isohreshold)
            {
                NP_ASSERT(isohreshold >= 0.0 && isohreshold <= 1.0,
                          "isothreshold must be on range [0, 1]");
                
                Image<bl, SIZE> visited(false);
                math::fltPointVector3D* shapes_with_holes;
                shapes_with_holes = new math::fltPointVector3D;
                
                container::vector<DmsLineSegmentUset>* temp_return = new container::vector<DmsLineSegmentUset>;
                
                for (ui16 y=0; y<base::GetHeight(); y++)
                {
                    for (ui16 x=0; x<base::GetWidth(); x++)
                    {
                        if (GetIsovalue(x, y) < isohreshold)
                        {
                            continue;
                        }
                        
                        if (visited.GetValue(x, y))
                        {
                            continue;
                        }
                        
                        //math::ui16PointUset region;
                        //flood fill into region AND mark each as visited
                        
                        math::ui16PointUset edgePoints;
                        //loop through region and grab points with a neighbor where GetIsovalue() < isohreshold
                        
                        math::ui16PointQueue floodFillQueue;
                        floodFillQueue.push(math::ui16Point{.x=x, .y=y});
                        
                        while(!floodFillQueue.empty())
                        {
                            //should be use == or >= below? for now, >= will do since we skip values < isovalue above
                            math::ui16Point front = floodFillQueue.front();
                            
                            if (GetIsovalue(front) >= isohreshold)
                            {
                                //determine if (x, y) is an edge point
                                if (IsIsovalueEdgePoint(front, isohreshold))
                                {
                                    edgePoints.insert(front);
                                }
                                
                                if (!visited.GetValue(front))
                                {
                                    visited.SetValue(front, true);
                                    
                                    if (front.y + 1 < base::GetHeight())
                                    {
                                        if (!visited.GetValue(front.x, front.y + 1))
                                        {
                                            floodFillQueue.push(math::ui16Point
                                            {
                                                .x=front.x,
                                                .y=static_cast<ui16>(front.y + 1)
                                            });
                                        }
                                    }
                                    if (front.y - 1 >= 0)
                                    {
                                        if (!visited.GetValue(front.x, front.y - 1))
                                        {
                                            floodFillQueue.push(math::ui16Point
                                            {
                                                .x=front.x,
                                                .y=static_cast<ui16>(front.y - 1)
                                            });
                                        }
                                    }
                                    if (front.x + 1 < base::GetWidth())
                                    {
                                        if (!visited.GetValue(front.x + 1, front.y))
                                        {
                                            floodFillQueue.push(math::ui16Point
                                            {
                                                .x=static_cast<ui16>(front.x + 1),
                                                .y=front.y
                                            });
                                        }
                                    }
                                    if (floodFillQueue.front().x - 1 >= 0)
                                    {
                                        if (!visited.GetValue(front.x - 1, front.y))
                                        {
                                            floodFillQueue.push(math::ui16Point
                                            {
                                                .x=static_cast<ui16>(front.x - 1),
                                                .y=front.y
                                            });
                                        }
                                    }
                                }
                            }
                            
                            floodFillQueue.pop();
                        }
                        
                        //---------------------------------------------------------------------------
                        
                        DmsLineSegmentUset* segments = new DmsLineSegmentUset;
                        
                        for (math::ui16Point point : edgePoints)
                        {
                            //gather line segments from edge points
                            GetLineSegmentsAroundPoint(*segments, point, isohreshold);
                        }
                        
                        //---------------------------------------------------------------------------
                        //temp step to just move on:
                        
                        DmsLineSegmentUset* dms_seg= new DmsLineSegmentUset;
                        
                        for (DmsLineSegment seg : *segments)
                        {
                            dms_seg->insert(DmsLineSegment(seg.Begin(), seg.End(), seg.AssociatedPoint()));
                        }
                        
                        temp_return->push_back(*dms_seg);
                        
                        //---------------------------------------------------------------------------
                        
                        DmsLineSegmentVector2D line_loops;
                        bl enable = true;
                        
                        if (segments->size() > 0)
                        {
                            //get line loops from ordering line segments
                            typename DmsLineSegmentUset::iterator seg_iterator;
                            seg_iterator = segments->begin();
                            
                            typename DmsLineSegmentVector2D::iterator loop_iterator;
                            line_loops.push_back(DmsLineSegmentVector());
                            loop_iterator = --line_loops.end();
                            
                            loop_iterator->push_back(*seg_iterator);
                            seg_iterator = segments->erase(seg_iterator);
                            
                            //sort the dms line segments into a connected loop
                            while (enable && !segments->empty())
                            {
                                bl found_loop = false;
                                bl found_next = false;
                                for (DmsLineSegment seg : *segments)
                                {
                                    if (seg.Begin() == loop_iterator->back().Begin() ||
                                        seg.Begin() == loop_iterator->back().End() ||
                                        seg.End() == loop_iterator->back().Begin() ||
                                        seg.End() == loop_iterator->back().End())
                                    {
                                        //seg matches up on back's begin or end
                                        loop_iterator->push_back(seg);
                                        segments->erase(seg);
                                        //seg_iterator = segments->erase(seg_iterator);
                                        
                                        found_next = true;
                                        break;
                                    }
                                }
                                
                                if (!found_next || segments->empty())
                                {
                                    //loop detection
                                    if (loop_iterator->back().Begin() == loop_iterator->front().Begin() ||
                                        loop_iterator->back().Begin() == loop_iterator->front().End() ||
                                        loop_iterator->back().End() == loop_iterator->front().Begin() ||
                                        loop_iterator->back().End() == loop_iterator->front().End())
                                    {
                                        //previous loop confirmed
                                        
                                        //if (seg.Begin == loop_iterator->front().begin ||
                                        //    seg.Begin == loop_iterator->front().end ||
                                        //    seg.End == loop_iterator->front().begin ||
                                        //    seg.End == loop_iterator->front().end)
                                        
                                        //seg matches up on front's begin or end
                                        //loop_iterator->push_back(seg);
                                        //segments->erase(seg);
                                        //seg_iterator = segments->erase(seg_iterator);
                                        
                                        //next loop check
                                        if (!segments->empty())
                                        {
                                            //next loop is confirmed
                                            line_loops.push_back(DmsLineSegmentVector());
                                            loop_iterator = --line_loops.end();
                                            
                                            //prime next loop
                                            seg_iterator = segments->begin();
                                            loop_iterator->push_back(*seg_iterator);
                                            segments->erase(seg_iterator);
                                        }
                                        
                                        found_loop = true;
                                    }
                                }
                            }
                            
                            if (enable && !segments->empty())
                            {
                                NP_ASSERT(false, "we should have points in our segments");
                            }
                            
                            ::std::sort(line_loops.begin(), line_loops.end(),
                                        [](const DmsLineSegmentVector &a,
                                           const DmsLineSegmentVector &b) -> bl
                                        { return a.size() > b.size(); });
                        }
                        
                        //---------------------------------------------------------------------------
                        
                        math::fltPointVector2D shapehen_holes;
                        //get the midpoints of all line segments to solidify our shape then holes
                        for (DmsLineSegmentVector loop : line_loops)
                        {
                            //TODO: are we able to not use a pointer??
                            math::fltPointVector* shape = new math::fltPointVector;
                            
                            for (DmsLineSegment segment : loop)
                            {
                                shape->push_back(segment.Midpoint());
                            }
                            
                            shapehen_holes.push_back(*shape);
                        }
                        
                        //sort the line loops based on how long they are [largest to shortest]
                        ::std::sort(shapehen_holes.begin(), shapehen_holes.end(),
                             [](const math::fltPointVector &a, const math::fltPointVector &b) -> bl
                        {
                            return a.size() > b.size();
                        });
                        
                        shapes_with_holes->push_back(shapehen_holes);
                        //save shapehen_holes to overall list
                    }
                }
                
                //add origin offset
                //TODO: put this logic above
                {
                    math::ui16Point origin = base::GetOrigin();
                    for (i32 i=0; i<shapes_with_holes->size(); i++)
                    {
                        for (i32 j=0; j<(*shapes_with_holes)[i].size(); j++)
                        {
                            for (i32 k=0; k<(*shapes_with_holes)[i][j].size(); k++)
                            {
                                (*shapes_with_holes)[i][j][k].x += origin.x;
                                (*shapes_with_holes)[i][j][k].y += origin.y;
                            }
                        }
                    }
                }
                
                return shapes_with_holes;
                //return temp_return;
            }
            
            /**
             serialize method
             */
            virtual ostrm& Insertion(ostrm& os, str filepath) const override
            {
                serialization::vendor::nlohmann::json json;
                str ui16_drawable_image_path = fs::append(fs::get_parent_path(filepath),
                                                             "ui16_drawable_image_path");
                json["ui16_drawable_image_path"] = ui16_drawable_image_path;
                
                os << json;//.dump(NP_JSON_SPACING);
                base::SaveTo(ui16_drawable_image_path);
                
                return os;
            }
            
            /**
             deserialize method
            */
            virtual istrm& Extraction(istrm& is, str filepath) override
            {
                serialization::vendor::nlohmann::json json;
                is >> json;
                
                str ui16_drawable_image_path = json["ui16_drawable_image_path"];
                base::LoadFrom(ui16_drawable_image_path);
                
                return is;
            }
            
            /**
             save oursellves inside the given dirpath
             return if the save was successful or not
             */
            virtual bl SaveTo(str dirpath) const override
            {
                return self_type::template SaveAs<self_type>(fs::append(dirpath, GetFilename()), this);
            }
            
            /**
             load outselves from the given dirpath
             return if the load was successful or not
             */
            virtual bl LoadFrom(str dirpath) override
            {
                return self_type::template LoadAs<self_type>(fs::append(dirpath, GetFilename()), this);
            }
        };
        
        template <ui16 SIZE>
        using ui8DmsImage = DmsImage<ui8, SIZE>;
        
        template <ui16 SIZE>
        using ui16DmsImage = DmsImage<ui16, SIZE>;
        
        template <ui16 SIZE>
        using ui32DmsImage = DmsImage<ui32, SIZE>;
        
        template <ui16 SIZE>
        using ui64DmsImage = DmsImage<ui64, SIZE>;
        
        template <ui16 SIZE>
        using i8DmsImage = DmsImage<i8, SIZE>;
        
        template <ui16 SIZE>
        using i16DmsImage = DmsImage<i16, SIZE>;
        
        template <ui16 SIZE>
        using i32DmsImage = DmsImage<i32, SIZE>;
        
        template <ui16 SIZE>
        using i64DmsImage = DmsImage<i64, SIZE>;
        
        template <ui16 SIZE>
        using fltDmsImage = DmsImage<flt, SIZE>;
        
        template <ui16 SIZE>
        using dblDmsImage = DmsImage<dbl, SIZE>;
        
        template <ui16 SIZE>
        using chrDmsImage = DmsImage<chr, SIZE>;
        
        template <ui16 SIZE>
        using uchrDmsImage = DmsImage<uchr, SIZE>;
        
        using ui8DmsImage32 = ui8DmsImage<32>;
        using ui8DmsImage64 = ui8DmsImage<64>;
        using ui8DmsImage128 = ui8DmsImage<128>;
        using ui8DmsImage256 = ui8DmsImage<256>;
        
        using ui16DmsImage32 = ui16DmsImage<32>;
        using ui16DmsImage64 = ui16DmsImage<64>;
        using ui16DmsImage128 = ui16DmsImage<128>;
        using ui16DmsImage256 = ui16DmsImage<256>;
        
        using ui32DmsImage32 = ui32DmsImage<32>;
        using ui32DmsImage64 = ui32DmsImage<64>;
        using ui32DmsImage128 = ui32DmsImage<128>;
        using ui32DmsImage256 = ui32DmsImage<256>;
        
        using ui64DmsImage32 = ui64DmsImage<32>;
        using ui64DmsImage64 = ui64DmsImage<64>;
        using ui64DmsImage128 = ui64DmsImage<128>;
        using ui64DmsImage256 = ui64DmsImage<256>;
        
        using i8DmsImage32 = i8DmsImage<32>;
        using i8DmsImage64 = i8DmsImage<64>;
        using i8DmsImage128 = i8DmsImage<128>;
        using i8DmsImage256 = i8DmsImage<256>;
        
        using i16DmsImage32 = i16DmsImage<32>;
        using i16DmsImage64 = i16DmsImage<64>;
        using i16DmsImage128 = i16DmsImage<128>;
        using i16DmsImage256 = i16DmsImage<256>;
        
        using i32DmsImage32 = i32DmsImage<32>;
        using i32DmsImage64 = i32DmsImage<64>;
        using i32DmsImage128 = i32DmsImage<128>;
        using i32DmsImage256 = i32DmsImage<256>;
        
        using i64DmsImage32 = i64DmsImage<32>;
        using i64DmsImage64 = i64DmsImage<64>;
        using i64DmsImage128 = i64DmsImage<128>;
        using i64DmsImage256 = i64DmsImage<256>;
        
        using fltDmsImage32 = fltDmsImage<32>;
        using fltDmsImage64 = fltDmsImage<64>;
        using fltDmsImage128 = fltDmsImage<128>;
        using fltDmsImage256 = fltDmsImage<256>;
        
        using dblDmsImage32 = dblDmsImage<32>;
        using dblDmsImage64 = dblDmsImage<64>;
        using dblDmsImage128 = dblDmsImage<128>;
        using dblDmsImage256 = dblDmsImage<256>;
        
        using chrDmsImage32 = chrDmsImage<32>;
        using chrDmsImage64 = chrDmsImage<64>;
        using chrDmsImage128 = chrDmsImage<128>;
        using chrDmsImage256 = chrDmsImage<256>;
        
        using uchrDmsImage32 = uchrDmsImage<32>;
        using uchrDmsImage64 = uchrDmsImage<64>;
        using uchrDmsImage128 = uchrDmsImage<128>;
        using uchrDmsImage256 = uchrDmsImage<256>;
    }
}

#endif /* NP_ENGINE_DMS_IMAGE_HPP */
