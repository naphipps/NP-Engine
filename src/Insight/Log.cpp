//
//  Log.cpp
//  Project Space
//
//  Created by Nathan Phipps on 5/21/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#include "Log.hpp"

namespace np
{
    namespace insight
    {
        atm_bl Log::_initialized(false);
        ::std::shared_ptr<spdlog::logger> Log::_file_logger;
        ::std::shared_ptr<spdlog::logger> Log::_stdout_logger;
        ::std::shared_ptr<spdlog::logger> Log::_logger;
    }
}
