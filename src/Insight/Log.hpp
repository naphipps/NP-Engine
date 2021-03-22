//
//  Log.hpp
//  Project Space
//
//  Created by Nathan Phipps on 5/21/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_LOG_HPP
#define NP_ENGINE_LOG_HPP

#include <memory>

#include "Foundation/Foundation.hpp"
#include "Container/Container.hpp"
#include "Primitive/Primitive.hpp"
#include "Filesystem/Filesystem.hpp"

//TODO: change all vendor code to use <> instead of ""
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace np
{
    namespace insight
    {
        /**
         Log class will allow us to log to file and stdout appropriately
         */
        class Log
        {
        private:
            static atm_bl _initialized;
            static ::std::shared_ptr<spdlog::logger> _file_logger;
            static ::std::shared_ptr<spdlog::logger> _stdout_logger;
            static ::std::shared_ptr<spdlog::logger> _logger;
            
        public:
            
            /**
             initializes the private loggers
             */
            static inline void Init()
            {
                if (!_initialized.load(mo_acquire))
                {
                    _initialized.store(true, mo_release);
                    
                    container::vector<spdlog::sink_ptr> sinks;
                    
                    //stdout sink
                    sinks.emplace_back(::std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
                    
                    //file sink
                    str log_filepath = fs::append(OUTPUT_DIR, "np.log");
                    sinks.emplace_back(::std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_filepath, true));
                    
                    str pattern = "[pid:%P, tid:%t] %+";
                    
                    sinks.front()->set_pattern("%^" + pattern + "%$");
                    sinks.back()->set_pattern(pattern);
                    
                    //constructor accepts multiple sinks via iterators begin and end
                    _logger = ::std::make_shared<spdlog::logger>("NP_LOG", sinks.begin(), sinks.end());
                    spdlog::register_logger(_logger);
                    _logger->set_level(spdlog::level::trace);
                    _logger->flush_on(spdlog::level::trace);
                    
                    _file_logger = ::std::make_shared<spdlog::logger>("NP_FILE", sinks.back());
                    spdlog::register_logger(_file_logger);
                    _file_logger->set_level(spdlog::level::trace);
                    _file_logger->flush_on(spdlog::level::trace);
                    
                    _stdout_logger = ::std::make_shared<spdlog::logger>("NP_STDOUT", sinks.front());
                    spdlog::register_logger(_stdout_logger);
                    _stdout_logger->set_level(spdlog::level::trace);
                    _stdout_logger->flush_on(spdlog::level::trace);
                }
            }
            
            /**
             gets the file logger
             */
            static inline ::std::shared_ptr<spdlog::logger>& GetFileLogger()
            {
                Init();
                return _file_logger;
            }
            
            /**
             gets the ::stdout logger
             */
            static inline ::std::shared_ptr<spdlog::logger>& GetStdoutLogger()
            {
                Init();
                return _stdout_logger;
            }
            
            /**
             gets the logger that goes to ::stdout and to file
             */
            static inline ::std::shared_ptr<spdlog::logger>& GetLogger()
            {
                Init();
                return _logger;
            }
        };
    }
}

#ifndef NP_LOG_ENABLE
#define NP_LOG_ENABLE 0
#warning NP_LOG_ENABLE is defined as 0
#else
#warning NP_LOG_ENABLE is already defined
#endif

#if NP_LOG_ENABLE

#define NP_LOG_TRACE(...) ::np::insight::Log::GetLogger()->trace(__VA_ARGS__)
#define NP_LOG_INFO(...) ::np::insight::Log::GetLogger()->info(__VA_ARGS__)
#define NP_LOG_WARN(...) ::np::insight::Log::GetLogger()->warn(__VA_ARGS__)
#define NP_LOG_ERROR(...) ::np::insight::Log::GetLogger()->error(__VA_ARGS__)
#define NP_LOG_CRITICAL(...) ::np::insight::Log::GetLogger()->critical(__VA_ARGS__)

#define NP_LOG_FILE_TRACE(...) ::np::insight::Log::GetFileLogger()->trace(__VA_ARGS__)
#define NP_LOG_FILE_INFO(...) ::np::insight::Log::GetFileLogger()->info(__VA_ARGS__)
#define NP_LOG_FILE_WARN(...) ::np::insight::Log::GetFileLogger()->warn(__VA_ARGS__)
#define NP_LOG_FILE_ERROR(...) ::np::insight::Log::GetFileLogger()->error(__VA_ARGS__)
#define NP_LOG_FILE_CRITICAL(...) ::np::insight::Log::GetFileLogger()->critical(__VA_ARGS__)

#define NP_LOG_STDOUT_TRACE(...) ::np::insight::Log::GetStdoutLogger()->trace(__VA_ARGS__)
#define NP_LOG_STDOUT_INFO(...) ::np::insight::Log::GetStdoutLogger()->info(__VA_ARGS__)
#define NP_LOG_STDOUT_WARN(...) ::np::insight::Log::GetStdoutLogger()->warn(__VA_ARGS__)
#define NP_LOG_STDOUT_ERROR(...) ::np::insight::Log::GetStdoutLogger()->error(__VA_ARGS__)
#define NP_LOG_STDOUT_CRITICAL(...) ::np::insight::Log::GetStdoutLogger()->critical(__VA_ARGS__)

#else

#define NP_LOG_TRACE(...)
#define NP_LOG_INFO(...)
#define NP_LOG_WARN(...)
#define NP_LOG_ERROR(...)
#define NP_LOG_CRITICAL(...)

#define NP_LOG_FILE_TRACE(...)
#define NP_LOG_FILE_INFO(...)
#define NP_LOG_FILE_WARN(...)
#define NP_LOG_FILE_ERROR(...)
#define NP_LOG_FILE_CRITICAL(...)

#define NP_LOG_STDOUT_TRACE(...)
#define NP_LOG_STDOUT_INFO(...)
#define NP_LOG_STDOUT_WARN(...)
#define NP_LOG_STDOUT_ERROR(...)
#define NP_LOG_STDOUT_CRITICAL(...)

#endif //NP_LOG_ENABLE

#endif /* NP_ENGINE_LOG_HPP */
