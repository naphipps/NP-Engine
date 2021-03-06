//
//  Image.hpp
//  Project Space
//
//  Created by Nathan Phipps on 5/17/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_IMAGE_HPP
#define NP_ENGINE_IMAGE_HPP

#include "TypeTraits/TypeTraits.hpp"
#include "Serialization/Serialization.hpp"
#include "Insight/Insight.hpp"
#include "Primitive/Primitive.hpp"
#include "Container/Container.hpp"
#include "Math/Math.hpp"

namespace np
{
    namespace core
    {
        /**
         Image is to be treated as a 2D array of ui16 elements
         Image inner array is created on init method
         */
        template <typename T, ui16 SIZE>
        class Image : public serialization::Serializable //TODO: I think we should move this to some sort of Image-based project??
        {
        private:
            
            NP_STATIC_ASSERT((typetraits::IsSame<T, ui8> ||
                              typetraits::IsSame<T, ui16> ||
                              typetraits::IsSame<T, ui32> ||
                              typetraits::IsSame<T, ui64> ||
                              typetraits::IsSame<T, i8> ||
                              typetraits::IsSame<T, i16> ||
                              typetraits::IsSame<T, i32> ||
                              typetraits::IsSame<T, i64> ||
                              typetraits::IsSame<T, flt> ||
                              typetraits::IsSame<T, dbl> ||
                              typetraits::IsSame<T, chr> ||
                              typetraits::IsSame<T, uchr> ||
                              typetraits::IsSame<T, bl> ||
                              typetraits::IsBaseOf<serialization::Serializable, T>),
                             "Image<T, SIZE> requires T to be types listed in Image.hpp");
            
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
                    filename = "ui" + to_str(8 * sizeof(T)) + "Image" + to_str(SIZE) + ".bin";
                }
                else if constexpr (typetraits::IsSame<T, i8> ||
                                   typetraits::IsSame<T, i16> ||
                                   typetraits::IsSame<T, i32> ||
                                   typetraits::IsSame<T, i64>)
                {
                    filename = "i" + to_str(8 * sizeof(T)) + "Image" + to_str(SIZE) + ".bin";
                }
                NP_ELIF_IS_SAME(T, flt)
                {
                    filename = "fltImage" + to_str(SIZE) + ".bin";
                }
                NP_ELIF_IS_SAME(T, dbl)
                {
                    filename = "dblImage" + to_str(SIZE) + ".bin";
                }
                NP_ELIF_IS_SAME(T, chr)
                {
                    filename = "chrImage" + to_str(SIZE) + ".bin";
                }
                NP_ELIF_IS_SAME(T, uchr)
                {
                    filename = "uchrImage" + to_str(SIZE) + ".bin";
                }
                NP_ELIF_IS_SAME(T, bl)
                {
                    filename = "blImage" + to_str(SIZE) + ".bin";
                }
                NP_ELIF_IS_BASE_OF(T, serialization::Serializable)
                {
                    filename = "Image.json";
                }
                
                return filename;
            }
            
            constexpr static ui16 _width = SIZE;
            constexpr static ui16 _height = SIZE;
            container::array<T, (ui32)SIZE * (ui32)SIZE> _value;
            
        public:
            
            /**
             constructor; image is all zeros
             */
            Image()
            {
                Clear();
            }
            
            /**
             constructor; image is set to given value
             */
            Image(const T value)
            {
                for (ui32 i=0; i<_value.size(); i++)
                {
                    _value[i] = value;
                }
            }
            
            /**
             deconstructor
             */
            virtual ~Image()
            {}
            
            /**
             clears image to zeros
             */
            virtual inline void Clear()
            {
                if constexpr (typetraits::IsSame<T, ui8> ||
                              typetraits::IsSame<T, ui16> ||
                              typetraits::IsSame<T, ui32> ||
                              typetraits::IsSame<T, ui64> ||
                              typetraits::IsSame<T, i8> ||
                              typetraits::IsSame<T, i16> ||
                              typetraits::IsSame<T, i32> ||
                              typetraits::IsSame<T, i64> ||
                              typetraits::IsSame<T, flt> ||
                              typetraits::IsSame<T, dbl> ||
                              typetraits::IsSame<T, chr> ||
                              typetraits::IsSame<T, uchr> ||
                              typetraits::IsSame<T, bl>)
                {
                    for (ui32 i=0; i<_value.size(); i++)
                    {
                        _value[i] = (T)0;
                    }
                }
            }
            
            /**
             gets the width of the image
             */
            inline ui16 GetWidth() const
            {
                return _width;
            }
            
            /**
             gets the height of the image
             */
            inline ui16 GetHeight() const
            {
                return _height;
            }
            
            /**
             gets the value at given point
             */
            inline virtual T GetValue(const math::ui16Point point) const
            {
                return GetValue(point.x, point.y);
            }
            
            /**
             gets the value at given (x, y)
             */
            inline virtual T GetValue(const ui16 x, const ui16 y) const
            {
                return _value[x + _width * y];
            }
            
            /**
             sets the value at given point
             */
            inline virtual void SetValue(const math::ui16Point point, const T& value)
            {
                SetValue(point.x, point.y, value);
            }
            
            /**
             sets the given value at given (x, y)
             */
            inline virtual void SetValue(const ui16 x, const ui16 y, const T& value)
            {
                _value[x + _width * y] = value;
            }
            
            /**
             serialize method
             */
            virtual ostrm& Insertion(ostrm& os, str filepath) const override
            {
                if constexpr (typetraits::IsSame<T, ui8> ||
                              typetraits::IsSame<T, ui16> ||
                              typetraits::IsSame<T, ui32> ||
                              typetraits::IsSame<T, ui64> ||
                              typetraits::IsSame<T, i8> ||
                              typetraits::IsSame<T, i16> ||
                              typetraits::IsSame<T, i32> ||
                              typetraits::IsSame<T, i64> ||
                              typetraits::IsSame<T, flt> ||
                              typetraits::IsSame<T, dbl> ||
                              typetraits::IsSame<T, chr> ||
                              typetraits::IsSame<T, uchr> ||
                              typetraits::IsSame<T, bl>)
                {
                    //TODO: test to make sure this works - especially for flt and dbl
//                    T* data_ptr = const_cast<T*>(_value.data());
//                    chr* bytes = data_ptr;
//                    ui32 bytes_size = _value.size() * sizeof(T);
//                    os.write(bytes, bytes_size);
                    
                    for (ui32 i=0; i<_value.size(); i++)
                    {
                        os << _value[i];
                    }
                }
                else if constexpr (typetraits::IsBaseOf<T, serialization::Serializable>)
                {
                    str dirpath = fs::get_parent_path(filepath);
                    serialization::vendor::nlohmann::json json;
                    
                    for (ui32 i=0; i<_value.size(); i++)
                    {
                        str i_dir = to_str(i);
                        json[i_dir] = fs::append(dirpath, i_dir);
                        _value[i].SaveTo(json[i_dir]);
                    }
                    
                    os << json.dump(NP_JSON_SPACING);
                }
                
                return os;
            }
            
            /**
             deserialize method
            */
            virtual istrm& Extraction(istrm& is, str filepath) override
            {
                if constexpr (typetraits::IsSame<T, ui8> ||
                              typetraits::IsSame<T, ui16> ||
                              typetraits::IsSame<T, ui32> ||
                              typetraits::IsSame<T, ui64> ||
                              typetraits::IsSame<T, i8> ||
                              typetraits::IsSame<T, i16> ||
                              typetraits::IsSame<T, i32> ||
                              typetraits::IsSame<T, i64> ||
                              typetraits::IsSame<T, flt> ||
                              typetraits::IsSame<T, dbl> ||
                              typetraits::IsSame<T, chr> ||
                              typetraits::IsSame<T, uchr> ||
                              typetraits::IsSame<T, bl>)
                {
                    //TODO: test to make sure this works - especially for flt and dbl
//                    const chr* const_bytes = static_cast<const chr*>(_value.data());
//                    chr* bytes = const_cast<chr*>(const_bytes);
//                    ui32 bytes_size = _value.size() * sizeof(T);
//                    is.read(bytes, bytes_size);
                    
                    for (ui32 i=0; i<_value.size(); i++)
                    {
                        is >> _value[i];
                    }
                }
                else if constexpr (typetraits::IsBaseOf<T, serialization::Serializable>)
                {
                    serialization::vendor::nlohmann::json json;
                    is >> json;
                    
                    for (ui32 i=0; i<_value.size(); i++)
                    {
                        str i_dir = to_str(i);
                        _value[i].LoadFrom(json[i_dir]);
                    }
                }
                
                return is;
            }
            
            /**
             save oursellves inside the given dirpath
             return if the save was successful or not
             */
            virtual bl SaveTo(str dirpath) const override
            {
                return Image<T, SIZE>::template SaveAs<Image<T, SIZE>>(fs::append(dirpath, GetFilename()), this);
            }
            
            /**
             load outselves from the given dirpath
             return if the load was successful or not
             */
            virtual bl LoadFrom(str dirpath) override
            {
                return Image<T, SIZE>::template LoadAs<Image<T, SIZE>>(fs::append(dirpath, GetFilename()), this);
            }
        };
        
        template <ui16 SIZE>
        using ui8Image = Image<ui8, SIZE>;
        
        template <ui16 SIZE>
        using ui16Image = Image<ui16, SIZE>;
        
        template <ui16 SIZE>
        using ui32Image = Image<ui32, SIZE>;
        
        template <ui16 SIZE>
        using ui64Image = Image<ui64, SIZE>;
        
        template <ui16 SIZE>
        using i8Image = Image<i8, SIZE>;
        
        template <ui16 SIZE>
        using i16Image = Image<i16, SIZE>;
        
        template <ui16 SIZE>
        using i32Image = Image<i32, SIZE>;
        
        template <ui16 SIZE>
        using i64Image = Image<i64, SIZE>;
        
        template <ui16 SIZE>
        using fltImage = Image<flt, SIZE>;

        template <ui16 SIZE>
        using dblImage = Image<dbl, SIZE>;

        template <ui16 SIZE>
        using chrImage = Image<chr, SIZE>;

        template <ui16 SIZE>
        using uchrImage = Image<uchr, SIZE>;
        
        template <ui16 SIZE>
        using blImage = Image<bl, SIZE>;

        using ui8Image32 = ui8Image<32>;
        using ui8Image64 = ui8Image<64>;
        using ui8Image128 = ui8Image<128>;
        using ui8Image256 = ui8Image<256>;
        
        using ui16Image32 = ui16Image<32>;
        using ui16Image64 = ui16Image<64>;
        using ui16Image128 = ui16Image<128>;
        using ui16Image256 = ui16Image<256>;
        
        using ui32Image32 = ui32Image<32>;
        using ui32Image64 = ui32Image<64>;
        using ui32Image128 = ui32Image<128>;
        using ui32Image256 = ui32Image<256>;
        
        using ui64Image32 = ui64Image<32>;
        using ui64Image64 = ui64Image<64>;
        using ui64Image128 = ui64Image<128>;
        using ui64Image256 = ui64Image<256>;
        
        using i8Image32 = i8Image<32>;
        using i8Image64 = i8Image<64>;
        using i8Image128 = i8Image<128>;
        using i8Image256 = i8Image<256>;
        
        using i16Image32 = i16Image<32>;
        using i16Image64 = i16Image<64>;
        using i16Image128 = i16Image<128>;
        using i16Image256 = i16Image<256>;
        
        using i32Image32 = i32Image<32>;
        using i32Image64 = i32Image<64>;
        using i32Image128 = i32Image<128>;
        using i32Image256 = i32Image<256>;
        
        using i64Image32 = i64Image<32>;
        using i64Image64 = i64Image<64>;
        using i64Image128 = i64Image<128>;
        using i64Image256 = i64Image<256>;
        
        using fltImage32 = fltImage<32>;
        using fltImage64 = fltImage<64>;
        using fltImage128 = fltImage<128>;
        using fltImage256 = fltImage<256>;
        
        using dblImage32 = dblImage<32>;
        using dblImage64 = dblImage<64>;
        using dblImage128 = dblImage<128>;
        using dblImage256 = dblImage<256>;
        
        using chrImage32 = chrImage<32>;
        using chrImage64 = chrImage<64>;
        using chrImage128 = chrImage<128>;
        using chrImage256 = chrImage<256>;
        
        using uchrImage32 = uchrImage<32>;
        using uchrImage64 = uchrImage<64>;
        using uchrImage128 = uchrImage<128>;
        using uchrImage256 = uchrImage<256>;
        
        using blImage32 = blImage<32>;
        using blImage64 = blImage<64>;
        using blImage128 = blImage<128>;
        using blImage256 = blImage<256>;
    }
}

#endif /* NP_ENGINE_IMAGE_HPP */
