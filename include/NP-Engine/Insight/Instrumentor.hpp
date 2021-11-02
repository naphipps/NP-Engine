//
//  Instrumentor.hpp
//  Project Space
//
//  Created by Nathan Phipps on 5/19/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_INSTRUMENTOR_HPP
#define NP_ENGINE_INSTRUMENTOR_HPP

#include <fstream>
#include <mutex>
#include <thread>
#include <sstream>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Time/Time.hpp"
#include "NP-Engine/Filesystem/Filesystem.hpp"
#include "NP-Engine/Serialization/Serialization.hpp"

#include "Timer.hpp"
#include "ScopedTimer.hpp"

namespace np
{
    namespace insight
    {
        /**
         trace_event will be used by the Instrumentor to store event details that we want to trace
         */
        struct TraceEvent
        {
            str Name;
            time::SteadyTimestamp StartTimestamp;
            time::DurationMicroseconds ElapsedMicroseconds;
            time::DurationMilliseconds ElapsedMilliseconds;
            ::std::thread::id ThreadId;
        };
        
        /**
         Instrumentor will keep track of all given trace_events and save them to our given filepath during every trace event add and deconstruction
         we will save the trace events in a json file
         */
        class Instrumentor
        {
        private:
            static const str OtherData;
            static const str TraceEvents;
            static Instrumentor _instance;
            
            serialization::vendor::nlohmann::json _json;
            str _filepath;
            Timer _trace_timer;
            ::std::mutex _mutex;
            
            /**
             private constructor - intentional singleton
             */
            Instrumentor():
            Instrumentor(fs::append(OUTPUT_DIR, "result.json"))
            {}
            
            /**
             private constructor - intentional singleton
             */
            Instrumentor(str filepath):
            _filepath(filepath)
            {
                _json[OtherData] = serialization::vendor::nlohmann::json::object();
                _json[TraceEvents] = serialization::vendor::nlohmann::json::array();
                _trace_timer.Stop();
            }
            
            /**
             saves the json file to our filepath
             */
            void InternalSave()
            {
                if (_filepath.size() > 0)
                {
                    ofstrm out_stream;
                    out_stream.open(_filepath);
                    
                    if (out_stream.is_open())
                    {
                        out_stream << _json;
                        out_stream.flush();
                        out_stream.close();
                    }
                }
            }
            
        public:
            
            /**
             singleton getter for the static instance
             */
            static Instrumentor& Get()
            {
                return _instance;
            }
            
            /**
             deconstructor
             */
            ~Instrumentor()
            {
                Save();
            }
            
            /**
             saves the json file to the filepath we already have or the given one if specified
             */
            void Save(const str filepath = "")
            {
                ::std::lock_guard<::std::mutex> lock(_mutex);
                
                if (filepath.size() > 0)
                {
                    SetFilepath(filepath);
                }
                
                InternalSave();
            }
            
            /**
             sets the filepath of the json file we will save
             */
            void SetFilepath(const str filepath)
            {
                _filepath = filepath;
            }
            
            /**
             gets the filepath of the json file we will save
             */
            str GetFilepath()
            {
                return _filepath;
            }
            
            /**
             adds the given trace event to our json file
             */
            void AddTraceEvent(TraceEvent& event, bl save = false)
            {
                ::std::lock_guard<::std::mutex> lock(_mutex);
                
                ::std::stringstream ss;
                serialization::vendor::nlohmann::json trace;
                
                trace["cat"] = "function";
                trace["dur"] = event.ElapsedMicroseconds.count();
                trace["name"] = event.Name;
                trace["ph"] = "X";
                trace["pid"] = "0";
                ss << event.ThreadId;
                trace["tid"] = ss.str();
                ss.str(""); //clear ss
                trace["ts"] = time::DurationMicroseconds(event.StartTimestamp.time_since_epoch()).count();
                
                _json[TraceEvents].push_back(trace);
                
#ifndef NP_PROFILE_FORCE_SAVE_ON_TRACE_ADD
                if (save)
#endif
                {
                    InternalSave();
                }
            }
        };
        
        /**
         InstrumentorTimer is a scoped timer that profiles our code by submitting a traced event to the Instrumentor upon deconstruction
         */
        class InstrumentorTimer : public ScopedTimer
        {
        private:
            
            /**
             the instrumentor timer action
             this collects the trace event context from given timer and current thread then adds the trace event to the Instrumentor
             */
            static void AddTraceEventAction(ScopedTimer& timer)
            {
                TraceEvent event;
                event.Name = timer.GetName();
                event.ElapsedMilliseconds = timer.GetElapsedMilliseconds();
                event.ElapsedMicroseconds = timer.GetElapsedMicroseconds();
                event.StartTimestamp = timer.GetStartTimestamp();
                event.ThreadId = ::std::this_thread::get_id();
                np::insight::Instrumentor::Get().AddTraceEvent(event);
            }
            
        public:
            
            /**
             constructor
             */
            InstrumentorTimer(str name):
            ScopedTimer(name, AddTraceEventAction)
            {}
            
            /**
             deconstructor
             */
            ~InstrumentorTimer()
            {}
        };
    }
}

//check if NP_PROFILE_ENABLE is defined
#ifndef NP_PROFILE_ENABLE
#define NP_PROFILE_ENABLE 0
#warning NP_PROFILE_ENABLE is defined as 0
#else
#warning NP_PROFILE_ENABLE is already defined
#endif

//set our Instrumentor-based macros
#if NP_PROFILE_ENABLE

#define NP_PROFILE_SCOPE(name) ::np::insight::InstrumentorTimer timer##__LINE__(name)
#define NP_PROFILE_FUNCTION() NP_PROFILE_SCOPE(NP_FUNCTION)
#define NP_PROFILE_SAVE() ::np::insight::Instrumentor::Get().Save();

#else

#define NP_PROFILE_SCOPE(name)
#define NP_PROFILE_FUNCTION()
#define NP_PROFILE_SAVE()

#endif //NP_PROFILE_ENABLE

#endif /* NP_ENGINE_INSTRUMENTOR_HPP */
