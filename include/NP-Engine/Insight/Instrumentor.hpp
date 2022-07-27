//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/19/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_INSTRUMENTOR_HPP
#define NP_ENGINE_INSTRUMENTOR_HPP

#include <memory>
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
#include "Log.hpp"

namespace np::insight
{
	class Instrumentor
	{
	private:
		static Mutex _mutex;
		static atm_bl _initialized;
		static ::std::shared_ptr<::rapidjson::Document> _report;
		static ::std::string _filepath;
		static atm_bl _save_on_trace;
		static atm_bl _enable_trace;

		static void SaveReport()
		{
			if (_filepath.size() > 0)
			{
				::np::insight::Log::GetLogger()->info("Saving Instrumentor Report...");

				::std::ofstream out_stream;
				out_stream.open(_filepath);

				if (out_stream.is_open())
				{
					::rapidjson::StringBuffer buffer;
					::rapidjson::Writer<::rapidjson::StringBuffer> writer(buffer);
					_report->Accept(writer);

					out_stream << buffer.GetString();
					out_stream.flush();
					out_stream.close();
				}

				::np::insight::Log::GetLogger()->info("Done Saving Instrumentor Report.");
			}
		}

		static void Init()
		{
			bl expected = false;
			if (_initialized.compare_exchange_strong(expected, true, mo_release, mo_relaxed))
			{
				_enable_trace = true;
				_save_on_trace = false;
				_filepath = fs::Append(fs::GetCurrentPath(), "profile_report.json");

				_report = ::std::make_shared<::rapidjson::Document>();
				_report->SetObject();

				::rapidjson::Value other_data_object;
				other_data_object.SetObject();
				_report->AddMember("otherData", other_data_object, _report->GetAllocator());

				::rapidjson::Value trace_events_array;
				trace_events_array.SetArray();
				_report->AddMember("traceEvents", trace_events_array, _report->GetAllocator());
			}
		}

	public:
		static void Reset()
		{
			Lock lock(_mutex);
			Init();
			SaveReport();
			_report.reset();
			_initialized.store(false, mo_release);
		}

		static void Save(const ::std::string filepath = "")
		{
			Lock lock(_mutex);
			Init();

			if (filepath.size() > 0)
				_filepath = filepath;

			SaveReport();
		}

		static void SetFilepath(const ::std::string filepath)
		{
			Lock lock(_mutex);
			Init();
			_filepath = filepath;
		}

		static ::std::string GetFilepath()
		{
			return _filepath;
		}

		static void EnableSaveOnTrace(bl enable = true)
		{
			Lock lock(_mutex);
			Init();
			_save_on_trace = enable;
		}

		static void EnableTraceAdd(bl enable = true)
		{
			Lock lock(_mutex);
			Init();
			_enable_trace = enable;
		}

		static void AddTraceEvent(TraceEvent& te)
		{
			Lock lock(_mutex);
			Init();

			if (_enable_trace)
			{
				::std::stringstream ss;
				ss << te.ThreadId;
				::std::string tid_str = ss.str();
				ss.str(""); // clear ss

				::rapidjson::MemoryPoolAllocator<::rapidjson::CrtAllocator>& allocator = _report->GetAllocator();
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

				(*_report)["traceEvents"].PushBack(::std::move(trace), allocator);

				if (_save_on_trace)
					SaveReport();
			}
		}
	};
} // namespace np::insight

#endif /* NP_ENGINE_INSTRUMENTOR_HPP */
