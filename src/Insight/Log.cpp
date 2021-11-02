//
//  Log.cpp
//  Project Space
//
//  Created by Nathan Phipps on 5/21/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#include "NP-Engine/Insight/Log.hpp"

namespace np
{
    namespace insight
    {
        atm_bl Log::_initialized(false);
        ::std::shared_ptr<spdlog::logger> Log::_file_logger;
        ::std::shared_ptr<spdlog::logger> Log::_stdout_logger;
        ::std::shared_ptr<spdlog::logger> Log::_logger;
        ::std::shared_ptr<::spdlog::sinks::sink> Log::_stdout_sink;
        ::std::shared_ptr<::spdlog::sinks::sink> Log::_file_sink;
    }
}
