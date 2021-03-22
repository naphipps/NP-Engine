//
//  DrawableImage.hpp
//  Project Space
//
//  Created by Nathan Phipps on 5/17/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_DRAWABLE_IMAGE_HPP
#define NP_ENGINE_DRAWABLE_IMAGE_HPP

#include "Primitive/Primitive.hpp"
#include "Container/Container.hpp"
#include "Math/Math.hpp"
#include "Serialization/Serialization.hpp"

#include "Image.hpp"

namespace np
{
    namespace core
    {
        /**
         DrawableImage is to be treated as a 2D array of ui16 elements and provides simple drawing methods
         DrawableImage inner array is created on construction
         */
        template <typename T, ui16 SIZE>
        class DrawableImage : public Image<T, SIZE>
        {
        private:
            
            using base = Image<T, SIZE>;
            
            math::ui16Point _origin;
            math::ui16AABB _aabb;
            
            constexpr static chr BaseImageDir[] = "base_image_dir";
            
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
                    filename = "ui" + to_str(8 * sizeof(T)) + "DrawableImage" + to_str(SIZE) + ".json";
                }
                else if constexpr (typetraits::IsSame<T, i8> ||
                                   typetraits::IsSame<T, i16> ||
                                   typetraits::IsSame<T, i32> ||
                                   typetraits::IsSame<T, i64>)
                {
                    filename = "i" + to_str(8 * sizeof(T)) + "DrawableImage" + to_str(SIZE) + ".json";
                }
                NP_ELIF_IS_SAME(T, flt)
                {
                    filename = "fltDrawableImage" + to_str(SIZE) + ".json";
                }
                NP_ELIF_IS_SAME(T, dbl)
                {
                    filename = "dblDrawableImage" + to_str(SIZE) + ".json";
                }
                NP_ELIF_IS_SAME(T, chr)
                {
                    filename = "chrDrawableImage" + to_str(SIZE) + ".json";
                }
                NP_ELIF_IS_SAME(T, uchr)
                {
                    filename = "uchrDrawableImage" + to_str(SIZE) + ".json";
                }
                NP_ELIF_IS_SAME(T, bl)
                {
                    filename = "blDrawableImage" + to_str(SIZE) + ".json";
                }
                NP_ELIF_IS_BASE_OF(T, serialization::Serializable)
                {
                    filename = "DrawableImage.json";
                }
                
                return filename;
            }
            
        public:
            
            /**
             constructor; image is all zeros
             */
            DrawableImage():
            base(),
            _origin(math::ui16Point{.x=0, .y=0}),
            _aabb(math::ui16AABB
            {
                .LowerLeft = _origin,
                .UpperRight = math::ui16Point
                {
                    .x=base::GetWidth(),
                    .y=base::GetHeight()
                }
            })
            {}
            
            /**
             constructor; setting origin to (0, 0) - sets default value of image to given value
             */
            DrawableImage(const ui16 value):
            base(value),
            _origin(math::ui16Point{.x=0, .y=0}),
            _aabb(math::ui16AABB
            {
                .LowerLeft = _origin,
                .UpperRight = math::ui16Point
                {
                    .x=base::GetWidth(),
                    .y=base::GetHeight()
                }
            })
            {}
            
            /**
             constructor; setting origin to given origin - sets default value of image to given value
             */
            DrawableImage(const ui16 value, const math::ui16Point origin):
            base(value),
            _origin(origin),
            _aabb(math::ui16AABB
            {
                .LowerLeft = _origin,
                .UpperRight = math::ui16Point
                {
                    .x=static_cast<ui16>(_origin.x + base::GetWidth()),
                    .y=static_cast<ui16>(_origin.y + base::GetHeight())
                }
            })
            {}
            
            /**
             constructor; setting origin to given origin - image is all zeros
             */
            DrawableImage(const math::ui16Point origin):
            base(),
            _origin(origin),
            _aabb(math::ui16AABB
            {
                .LowerLeft = _origin,
                .UpperRight = math::ui16Point
                {
                    .x=static_cast<ui16>(_origin.x + base::GetWidth()),
                    .y=static_cast<ui16>(_origin.y + base::GetHeight())
                }
            })
            {}
            
            /**
             deconstructor
             */
            virtual ~DrawableImage()
            {
                base::Clear();
            }
            
            /**
             sets the origin of the image
             */
            inline void SetOrigin(const math::ui16Point origin)
            {
                SetOrigin(origin.x, origin.y);
            }
            
            /**
             sets the origin of the image
             */
            inline void SetOrigin(const ui16 x, const ui16 y)
            {
                _origin.x = x;
                _origin.y = y;
                
                _aabb.LowerLeft = _origin;
                _aabb.UpperRight.x = _origin.x + base::GetWidth();
                _aabb.UpperRight.y = _origin.y + base::GetHeight();
            }
            
            /**
             gets the origin of the image
             */
            inline math::ui16Point GetOrigin() const
            {
                return _origin;
            }
            
            /**
             gets the aabb of the image
             */
            inline math::ui16AABB GetAABB() const
            {
                return _aabb;
            }
            
            /**
             gets the value at the given point
             */
            inline T GetValue(const math::ui16Point point) const override
            {
                return GetValue(point.x, point.y);
            }
            
            /**
             gets the value at the given (x, y)
             */
            inline T GetValue(const ui16 x, const ui16 y) const override
            {
                return base::GetValue(x - _origin.x, y - _origin.y);
            }
            
            /**
             sets the given value at given point
             */
            inline void SetValue(const math::ui16Point point, const T& value) override
            {
                SetValue(point.x, point.y, value);
            }
            
            /**
             sets the given value at given (x, y)
             */
            inline void SetValue(const ui16 x, const ui16 y, const T& value) override
            {
                base::SetValue(x - _origin.x, y - _origin.y, value);
            }
            
            /**
             draws a line from the given point a to given point b and given the line the given value
             */
            inline void DrawLine(const math::ui16Point a, const math::ui16Point b, const T& value)
            {
                //TODO: cleanup these bresenham calls?
                //TODO: we could move the bresenham from terrain image to here?
                math::ui16PointVector line = math::GetBresenhamLinePoints(a, b);
                
                for (math::ui16Point point : line)
                {
                    if (_aabb.Contains(point))
                    {
                        SetValue(point, value);
                    }
                }
            }
            
            /**
             draws the given lines with the given value, but if the point does not exist in this image then we don't draw it
             */
            inline void DrawLineLoop(const math::ui16PointVector &points, const T& value)
            {
                for (ui32 i=0; i<points.size()-1; i++)
                {
                    //get bresenham line, then draw on image values
                    math::ui16Point begin = points[i];
                    math::ui16Point end = points[i+1];
                    
                    math::ui16PointVector line = math::GetBresenhamLinePoints(begin, end);
                    
                    for (ui32 j=0; j<line.size(); j++)
                    {
                        if (_aabb.Contains(line[j]))
                        {
                            SetValue(line[j].x, line[j].y, value);
                        }
                    }
                }
                
                math::ui16PointVector line = math::GetBresenhamLinePoints(points.back(), points.front());
                
                for (ui32 i=0; i<line.size(); i++)
                {
                    if (_aabb.Contains(line[i]))
                    {
                        SetValue(line[i].x, line[i].y, value);
                    }
                }
            }
            
            /**
             flood fill atht the given point from the given old value to the given new value
             */
            inline void FloodFill(const math::ui16Point point, const T& oldValue, const T& newValue)
            {
                math::ui16PointQueue queue;
                queue.push(point);
                
                while (!queue.empty())
                {
                    math::ui16Point front = queue.front();
                    
                    if (GetValue(front) == oldValue)
                    {
                        SetValue(front, newValue);
                        math::ui16Point neighbor;
                        
                        //neighbor up
                        neighbor.x = front.x;
                        neighbor.y = front.y + 1;
                        
                        if (_aabb.Contains(neighbor))
                        {
                            if (neighbor.y > front.y) //don't wrap around image
                            {
                                if (GetValue(neighbor) == oldValue)
                                {
                                    queue.push(neighbor);
                                }
                            }
                        }
                        
                        //neighbor down
                        neighbor.x = front.x;
                        neighbor.y = front.y - 1;
                        
                        if (_aabb.Contains(neighbor))
                        {
                            if (neighbor.y < front.y) //don't wrap around image
                            {
                                if (GetValue(neighbor) == oldValue)
                                {
                                    queue.push(neighbor);
                                }
                            }
                        }
                        
                        //neighbor right
                        neighbor.x = front.x + 1;
                        neighbor.y = front.y;
                        
                        if (_aabb.Contains(neighbor))
                        {
                            if (neighbor.x > front.x) //don't wrap around image
                            {
                                if (GetValue(neighbor) == oldValue)
                                {
                                    queue.push(neighbor);
                                }
                            }
                        }
                        
                        //neighbor left
                        neighbor.x = front.x - 1;
                        neighbor.y = front.y;
                        
                        if (_aabb.Contains(neighbor))
                        {
                            if (neighbor.x < front.x) //don't wrap around image
                            {
                                if (GetValue(neighbor) == oldValue)
                                {
                                    queue.push(neighbor);
                                }
                            }
                        }
                    }
                    
                    queue.pop();
                }
            }
            
            /**
             flood fill at the given (x, y) from the given old value to the given new value
             */
            inline void FloodFill(const ui16 x, const ui16 y, const T& oldValue, const T& newValue)
            {
                FloodFill(math::ui16Point{.x=x, .y=y}, oldValue, newValue);
            }
            
            /**
             flood fill at the given point from the given old value to the given new value
             returns a vector of math::fltPoint describing the edge points of the outside border of this image
             */
            inline math::fltPointVector FloodFillWithNeighbors(const math::ui16Point point, const T& oldValue, const T& newValue)
            {
                math::fltPointVector neighbors;
                
                math::ui16PointQueue queue;
                queue.push(point);
                
                while (!queue.empty())
                {
                    math::ui16Point front = queue.front();
                    
                    if (GetValue(front) == oldValue)
                    {
                        SetValue(front, newValue);
                        math::ui16Point neighbor;
                        
                        //neighbor up
                        neighbor.x = front.x;
                        neighbor.y = front.y + 1;
                        
                        if (_aabb.Contains(neighbor))
                        {
                            if (neighbor.y > front.y) //don't wrap around image
                            {
                                if (GetValue(neighbor) == oldValue)
                                {
                                    queue.push(neighbor);
                                }
                            }
                        }
                        else
                        {
                            neighbors.push_back(math::fltPoint
                                                {
                                .x=(flt)front.x,
                                .y=static_cast<flt>((flt)front.y + 1.f)
                            });
                        }
                        
                        //neighbor down
                        neighbor.x = front.x;
                        neighbor.y = front.y - 1;
                        
                        if (_aabb.Contains(neighbor))
                        {
                            if (neighbor.y < front.y) //don't wrap around image
                            {
                                if (GetValue(neighbor) == oldValue)
                                {
                                    queue.push(neighbor);
                                }
                            }
                        }
                        else
                        {
                            neighbors.push_back(math::fltPoint
                                                {
                                .x=(flt)front.x,
                                .y=static_cast<flt>((flt)front.y - 1.f)
                            });
                        }
                        
                        //neighbor right
                        neighbor.x = front.x + 1;
                        neighbor.y = front.y;
                        
                        if (_aabb.Contains(neighbor))
                        {
                            if (neighbor.x > front.x) //don't wrap around image
                            {
                                if (GetValue(neighbor) == oldValue)
                                {
                                    queue.push(neighbor);
                                }
                            }
                        }
                        else
                        {
                            neighbors.push_back(math::fltPoint
                                                {
                                .x=static_cast<flt>((flt)front.x + 1.f),
                                .y=(flt)front.y
                            });
                        }
                        
                        //neighbor left
                        neighbor.x = front.x - 1;
                        neighbor.y = front.y;
                        
                        if (_aabb.Contains(neighbor))
                        {
                            if (neighbor.x < front.x) //don't wrap around image
                            {
                                if (GetValue(neighbor) == oldValue)
                                {
                                    queue.push(neighbor);
                                }
                            }
                        }
                        else
                        {
                            neighbors.push_back(math::fltPoint
                                                {
                                .x=static_cast<flt>((flt)front.x - 1.f),
                                .y=(flt)front.y
                            });
                        }
                    }
                    
                    queue.pop();
                }
                
                return neighbors;
            }
            
            /**
             flood fill at the given (x, y) from the given old value to the given new value
             returns a vector of math::fltPoint describing the edge points of the outside border of this image
             */
            inline math::fltPointVector FloodFillWithNeighbors(const ui16 x, const ui16 y, const T& oldValue, const T& newValue)
            {
                return FloodFillWithNeighbors(math::ui16Point{.x=x, .y=y}, oldValue, newValue);
            }
            
            /**
             serialize method
             */
            virtual ostrm& Insertion(ostrm& os, str filepath) const override
            {
                serialization::vendor::nlohmann::json json;
                json["origin"] = serialization::vendor::nlohmann::json::array();
                json["origin"].push_back(_origin.x);
                json["origin"].push_back(_origin.y);
                
                json["aabb"] = serialization::vendor::nlohmann::json::object();
                json["aabb"]["LowerLeft"] = serialization::vendor::nlohmann::json::array();
                json["aabb"]["LowerLeft"].push_back(_aabb.LowerLeft.x);
                json["aabb"]["LowerLeft"].push_back(_aabb.LowerLeft.y);
                
                json["aabb"]["UpperRight"] = serialization::vendor::nlohmann::json::array();
                json["aabb"]["UpperRight"].push_back(_aabb.UpperRight.x);
                json["aabb"]["UpperRight"].push_back(_aabb.UpperRight.y);
                
                json[BaseImageDir] = fs::append(fs::get_parent_path(filepath), BaseImageDir);
                
                os << json.dump(NP_JSON_SPACING);
                base::SaveTo(json[BaseImageDir]);
                
                return os;
            }
            
            /**
             deserialize method
            */
            virtual istrm& Extraction(istrm& is, str filepath) override
            {
                serialization::vendor::nlohmann::json json;
                is >> json;
                
                _origin.x = json["origin"][0];
                _origin.y = json["origin"][1];
                
                _aabb.LowerLeft.x = json["aabb"]["LowerLeft"][0];
                _aabb.LowerLeft.y = json["aabb"]["LowerLeft"][1];
                
                _aabb.UpperRight.x = json["aabb"]["UpperRight"][0];
                _aabb.UpperRight.y = json["aabb"]["UpperRight"][1];
                
                base::LoadFrom(json[BaseImageDir]);
                
                return is;
            }
            
            /**
             save oursellves inside the given dirpath
             return if the save was successful or not
             */
            virtual bl SaveTo(str dirpath) const override
            {
                return DrawableImage<T, SIZE>::template SaveAs<DrawableImage<T, SIZE>>(fs::append(dirpath, GetFilename()), this);
            }
            
            /**
             load outselves from the given dirpath
             return if the load was successful or not
             */
            virtual bl LoadFrom(str dirpath) override
            {
                return DrawableImage<T, SIZE>::template LoadAs<DrawableImage<T, SIZE>>(fs::append(dirpath, GetFilename()), this);
            }
        };
        
        template <ui16 SIZE>
        using ui8DrawableImage = DrawableImage<ui8, SIZE>;
        
        template <ui16 SIZE>
        using ui16DrawableImage = DrawableImage<ui16, SIZE>;
        
        template <ui16 SIZE>
        using ui32DrawableImage = DrawableImage<ui32, SIZE>;
        
        template <ui16 SIZE>
        using ui64DrawableImage = DrawableImage<ui64, SIZE>;
        
        template <ui16 SIZE>
        using i8DrawableImage = DrawableImage<i8, SIZE>;
        
        template <ui16 SIZE>
        using i16DrawableImage = DrawableImage<i16, SIZE>;
        
        template <ui16 SIZE>
        using i32DrawableImage = DrawableImage<i32, SIZE>;
        
        template <ui16 SIZE>
        using i64DrawableImage = DrawableImage<i64, SIZE>;
        
        template <ui16 SIZE>
        using fltDrawableImage = DrawableImage<flt, SIZE>;
        
        template <ui16 SIZE>
        using dblDrawableImage = DrawableImage<dbl, SIZE>;
        
        template <ui16 SIZE>
        using chrDrawableImage = DrawableImage<chr, SIZE>;
        
        template <ui16 SIZE>
        using uchrDrawableImage = DrawableImage<uchr, SIZE>;
        
        template <ui16 SIZE>
        using blDrawableImage = DrawableImage<bl, SIZE>;
        
        using ui8DrawableImage32 = ui8DrawableImage<32>;
        using ui8DrawableImage64 = ui8DrawableImage<64>;
        using ui8DrawableImage128 = ui8DrawableImage<128>;
        using ui8DrawableImage256 = ui8DrawableImage<256>;
        
        using ui16DrawableImage32 = ui16DrawableImage<32>;
        using ui16DrawableImage64 = ui16DrawableImage<64>;
        using ui16DrawableImage128 = ui16DrawableImage<128>;
        using ui16DrawableImage256 = ui16DrawableImage<256>;
        
        using ui32DrawableImage32 = ui32DrawableImage<32>;
        using ui32DrawableImage64 = ui32DrawableImage<64>;
        using ui32DrawableImage128 = ui32DrawableImage<128>;
        using ui32DrawableImage256 = ui32DrawableImage<256>;
        
        using ui64DrawableImage32 = ui64DrawableImage<32>;
        using ui64DrawableImage64 = ui64DrawableImage<64>;
        using ui64DrawableImage128 = ui64DrawableImage<128>;
        using ui64DrawableImage256 = ui64DrawableImage<256>;
        
        using i8DrawableImage32 = i8DrawableImage<32>;
        using i8DrawableImage64 = i8DrawableImage<64>;
        using i8DrawableImage128 = i8DrawableImage<128>;
        using i8DrawableImage256 = i8DrawableImage<256>;
        
        using i16DrawableImage32 = i16DrawableImage<32>;
        using i16DrawableImage64 = i16DrawableImage<64>;
        using i16DrawableImage128 = i16DrawableImage<128>;
        using i16DrawableImage256 = i16DrawableImage<256>;
        
        using i32DrawableImage32 = i32DrawableImage<32>;
        using i32DrawableImage64 = i32DrawableImage<64>;
        using i32DrawableImage128 = i32DrawableImage<128>;
        using i32DrawableImage256 = i32DrawableImage<256>;
        
        using i64DrawableImage32 = i64DrawableImage<32>;
        using i64DrawableImage64 = i64DrawableImage<64>;
        using i64DrawableImage128 = i64DrawableImage<128>;
        using i64DrawableImage256 = i64DrawableImage<256>;
        
        using fltDrawableImage32 = fltDrawableImage<32>;
        using fltDrawableImage64 = fltDrawableImage<64>;
        using fltDrawableImage128 = fltDrawableImage<128>;
        using fltDrawableImage256 = fltDrawableImage<256>;
        
        using dblDrawableImage32 = dblDrawableImage<32>;
        using dblDrawableImage64 = dblDrawableImage<64>;
        using dblDrawableImage128 = dblDrawableImage<128>;
        using dblDrawableImage256 = dblDrawableImage<256>;
        
        using chrDrawableImage32 = chrDrawableImage<32>;
        using chrDrawableImage64 = chrDrawableImage<64>;
        using chrDrawableImage128 = chrDrawableImage<128>;
        using chrDrawableImage256 = chrDrawableImage<256>;
        
        using uchrDrawableImage32 = uchrDrawableImage<32>;
        using uchrDrawableImage64 = uchrDrawableImage<64>;
        using uchrDrawableImage128 = uchrDrawableImage<128>;
        using uchrDrawableImage256 = uchrDrawableImage<256>;
        
        using blDrawableImage32 = blDrawableImage<32>;
        using blDrawableImage64 = blDrawableImage<64>;
        using blDrawableImage128 = blDrawableImage<128>;
        using blDrawableImage256 = blDrawableImage<256>;
    }
}

#endif /* NP_ENGINE_DRAWABLE_IMAGE_HPP */
