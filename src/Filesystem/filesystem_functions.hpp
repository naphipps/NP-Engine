//
//  filesystem_functions.hpp
//  Project Space
//
//  Created by Nathan Phipps on 5/26/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_FILESYSTEM_FUNCTIONS_HPP
#define NP_ENGINE_FILESYSTEM_FUNCTIONS_HPP

#include <filesystem>

#include "Primitive/Primitive.hpp"

namespace np
{
    namespace fs
    {
        /**
         checks if the given file or dir exists
         */
        static inline bl exists(str path_string)
        {
            bl exists = false;
            
            if (path_string.size() > 0)
            {
                ::std::filesystem::path path(path_string);
                exists = ::std::filesystem::exists(path_string);
            }
            
            return exists;
        }
        
        /**
         gets the parent path of the given file or dir path
         */
        static inline str get_parent_path(str path_string)
        {
            str parent_path;
            
            if (path_string.size() > 0)
            {
                ::std::filesystem::path path(path_string);
                parent_path = path.parent_path().c_str();
            }
                
            return parent_path;
        }
        
        /**
         appends the given path string to the given parent path
         */
        static inline str append(str parent_path, str path_string)
        {
            ::std::filesystem::path parent(parent_path);
            ::std::filesystem::path path(path_string);
            
            return (parent / path).c_str();
        }
        
        /**
         creates all the directories throughout the given dir path
         */
        static inline void create_directories(str dirpath)
        {
            if (dirpath.size() > 0)
            {
                ::std::filesystem::path path(dirpath);
                ::std::filesystem::create_directories(path);
            }
        }
    }
}

#endif /* NP_ENGINE_FILESYSTEM_FUNCTIONS_HPP */
