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

#include "NP-Engine/Vendor/JsonInclude.hpp"

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

		nlohmann::json _json;
		::std::string _filepath;
		Timer _trace_timer;
		Mutex _mutex;

		Instrumentor(): Instrumentor(fs::Append(fs::GetCurrentPath(), "result.json")) {}

		Instrumentor(::std::string filepath): _filepath(filepath)
		{
			_json[OtherData] = nlohmann::json::object();
			_json[TraceEvents] = nlohmann::json::array();
			_trace_timer.Stop();
		}

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
			nlohmann::json trace;

			trace["cat"] = "function";
			trace["dur"] = event.ElapsedMicroseconds.count();
			trace["name"] = event.Name;
			trace["ph"] = "X";
			trace["pid"] = "0";
			ss << event.ThreadId;
			trace["tid"] = ss.str();
			ss.str(""); // clear ss
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
