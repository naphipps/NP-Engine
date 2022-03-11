//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/19/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_INSTRUMENTOR_HPP
#define NP_ENGINE_INSTRUMENTOR_HPP

#include <fstream>
#include <thread>
#include <sstream>
#include <string>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Time/Time.hpp"
#include "NP-Engine/Filesystem/Filesystem.hpp"

#include "NP-Engine/Vendor/RapidJsonInclude.hpp"

#include "TraceEvent.hpp"
#include "Timer.hpp"

namespace np::insight
{
	class Instrumentor
	{
	private:
		static const ::std::string OtherData;
		static const ::std::string TraceEvents;
		static Instrumentor _instance;

		::rapidjson::Document _json;
		::std::string _filepath;
		Timer _trace_timer;
		Mutex _mutex;

		Instrumentor(): Instrumentor(fs::Append(fs::GetCurrentPath(), "result.json")) {}

		Instrumentor(::std::string filepath): _filepath(filepath)
		{
			_json[OtherData.c_str()].SetObject();
			_json[TraceEvents.c_str()].SetArray();
			_trace_timer.Stop();
		}

		void InternalSave()
		{
			if (_filepath.size() > 0)
			{
				::std::ofstream out_stream;
				out_stream.open(_filepath);

				if (out_stream.is_open())
				{
					::rapidjson::StringBuffer buffer;
					buffer.Clear(); // TODO: don't know if this is needed...
					::rapidjson::Writer<::rapidjson::StringBuffer> writer(buffer);
					_json.Accept(writer);

					out_stream << buffer.GetString();
					out_stream.flush();
					out_stream.close();
				}
			}
		}

	public:
		static Instrumentor& Get()
		{
			return _instance;
		}

		~Instrumentor()
		{
			Save();
		}

		void Save(const ::std::string filepath = "")
		{
			Lock lock(_mutex);

			if (filepath.size() > 0)
			{
				SetFilepath(filepath);
			}

			InternalSave();
		}

		void SetFilepath(const ::std::string filepath)
		{
			_filepath = filepath;
		}

		::std::string GetFilepath()
		{
			return _filepath;
		}

		void AddTraceEvent(TraceEvent& event, bl save = false)
		{
			Lock lock(_mutex);

			::std::stringstream ss;
			ss << event.ThreadId;
			::std::string tid = ss.str();
			ss.str(""); // clear ss
			::rapidjson::Document trace;
			trace.SetObject();

			trace["cat"].SetString("function", 8);
			trace["dur"].SetDouble(event.ElapsedMicroseconds.count());
			trace["name"].SetString(event.Name.c_str(), event.Name.size());
			trace["ph"].SetString("X", 1);
			trace["pid"].SetString("0", 1);
			trace["tid"].SetString(tid.c_str(), tid.size());
			trace["ts"].SetDouble(time::DurationMicroseconds(event.StartTimestamp.time_since_epoch()).count());

			_json[TraceEvents.c_str()].GetArray().PushBack(trace.GetObject(), _json.GetAllocator());

#ifndef NP_PROFILE_FORCE_SAVE_ON_TRACE_ADD
			if (save)
#endif
			{
				InternalSave();
			}
		}
	};
} // namespace np::insight

// check if NP_PROFILE_ENABLE is defined
#ifndef NP_PROFILE_ENABLE
	#define NP_PROFILE_ENABLE 0
#endif

#if NP_PROFILE_ENABLE
	#define NP_PROFILE_SAVE() ::np::insight::Instrumentor::Get().Save();
#else
	#define NP_PROFILE_SAVE()
#endif // NP_PROFILE_ENABLE

#endif /* NP_ENGINE_INSTRUMENTOR_HPP */
