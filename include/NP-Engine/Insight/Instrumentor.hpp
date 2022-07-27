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
#include <utility>

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
		//TODO: should we use the same setup as Log for our InstrumentorPtr??

		Mutex _mutex;
		::rapidjson::Document _json;
		bl _enable_trace_add;
		bl _save_on_trace;
		::std::string _filepath;
		Timer _trace_timer;
		
		void InternalSave()
		{
			if (_filepath.size() > 0)
			{
				::std::ofstream out_stream;
				out_stream.open(_filepath);

				if (out_stream.is_open())
				{
					::rapidjson::StringBuffer buffer;
					::rapidjson::Writer<::rapidjson::StringBuffer> writer(buffer);
					_json.Accept(writer);

					out_stream << buffer.GetString();
					out_stream.flush();
					out_stream.close();
				}
			}
		}

	public:

		Instrumentor() : Instrumentor(fs::Append(fs::GetCurrentPath(), "result.json")) {}

		Instrumentor(::std::string filepath) : _enable_trace_add(true), _save_on_trace(false), _filepath(filepath)
		{
			_json.SetObject();

			::rapidjson::Value other_data_object;
			other_data_object.SetObject();
			_json.AddMember("otherData", other_data_object, _json.GetAllocator());

			::rapidjson::Value trace_events_array;
			trace_events_array.SetArray();
			_json.AddMember("traceEvents", trace_events_array, _json.GetAllocator());

			_trace_timer.Stop();
		}

		void Save(const ::std::string filepath = "")
		{
			Lock lock(_mutex);

			if (filepath.size() > 0)
				_filepath = filepath;

			InternalSave();
		}

		void SetFilepath(const ::std::string filepath)
		{
			Lock lock(_mutex);
			_filepath = filepath; //TODO: make internal method for this?
		}

		::std::string GetFilepath() const
		{
			return _filepath;
		}

		void EnableSaveOnTrace(bl enable = true)
		{
			Lock lock(_mutex);
			_save_on_trace = enable;
		}

		void EnableTraceAdd(bl enable = true)
		{
			Lock lock(_mutex);
			_enable_trace_add = enable;
		}

		void AddTraceEvent(TraceEvent& te)
		{
			Lock lock(_mutex);

			if (_enable_trace_add)
			{
				::std::stringstream ss;
				ss << te.ThreadId;
				::std::string tid_str = ss.str();
				ss.str(""); // clear ss

				::rapidjson::MemoryPoolAllocator<::rapidjson::CrtAllocator>& allocator = _json.GetAllocator();
				::rapidjson::Value name(te.Name.c_str(), te.Name.size(), allocator);
				::rapidjson::Value tid(tid_str.c_str(), tid_str.size(), allocator);
				::rapidjson::Value trace;

				trace.SetObject();
				trace.AddMember("cat", "function", allocator);
				trace.AddMember("dur", te.ElapsedMicroseconds.count(), allocator);
				trace.AddMember("name", name, allocator);
				trace.AddMember("ph", "X", allocator);
				trace.AddMember("pid", "0", allocator);
				trace.AddMember("tid", tid, allocator);
				trace.AddMember("ts", time::DurationMicroseconds(te.StartTimestamp.time_since_epoch()).count(), allocator);

				_json["traceEvents"].PushBack(::std::move(trace), allocator);

				if (_save_on_trace)
					InternalSave();
			}
		}
	};

	namespace __detail
	{
		extern Instrumentor* IntrumentorPtr;
	}

	static void StopInstrumentor()
	{
		if (__detail::IntrumentorPtr)
		{
			__detail::IntrumentorPtr->Save();
			memory::Destroy<Instrumentor>(memory::DefaultTraitAllocator, __detail::IntrumentorPtr);
			__detail::IntrumentorPtr = nullptr;
		}
	}

	static void StartInstrumentor()
	{
		StopInstrumentor();
		__detail::IntrumentorPtr = memory::Create<Instrumentor>(memory::DefaultTraitAllocator);
	}

	static Instrumentor& GetInstrumentor()
	{
		NP_ENGINE_ASSERT(__detail::IntrumentorPtr, "Call StartInstrumentor() before GetInstrumentor().");
		return *__detail::IntrumentorPtr;
	}
} // namespace np::insight

#endif /* NP_ENGINE_INSTRUMENTOR_HPP */
