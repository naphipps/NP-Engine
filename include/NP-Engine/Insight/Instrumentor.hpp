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
#include "NP-Engine/FileSystem/FileSystem.hpp"

#include "NP-Engine/Vendor/RapidJsonInclude.hpp"

#include "TraceEvent.hpp"
#include "Timer.hpp"
#include "Log.hpp"

namespace np::nsit
{
	class Instrumentor
	{
	public:
		struct Properties
		{
			bl isInitialized;
			::std::shared_ptr<::rapidjson::Document> report;
			::std::string filepath;
			bl enableSaveOfTrace;
			bl enableTrace;
		};

	private:
		static mutexed_wrapper<Properties> _properties;

		static void SaveReport(Properties& properties)
		{
			if (properties.filepath.size() > 0)
			{
				Log::GetLogger()->info("Saving Instrumentor Profile Report...");

				::std::ofstream out_stream;
				out_stream.open(properties.filepath);

				if (out_stream.is_open())
				{
					::rapidjson::StringBuffer buffer;
					::rapidjson::PrettyWriter<::rapidjson::StringBuffer> writer(buffer);
					properties.report->Accept(writer);

					out_stream << buffer.GetString();
					out_stream.flush();
					out_stream.close();
				}

				Log::GetLogger()->info("Done Saving Instrumentor Profile Report: '" + properties.filepath + "'");
			}
		}

		static void Init(Properties& properties)
		{
			if (!properties.isInitialized)
			{
				properties.isInitialized = true;
				properties.enableTrace = true;
				properties.enableSaveOfTrace = false;
				properties.filepath = fsys::Append(fsys::GetCurrentPath(), "profile_report.json");

				properties.report = ::std::make_shared<::rapidjson::Document>();
				properties.report->SetObject();

				::rapidjson::Value other_data_object;
				other_data_object.SetObject();
				properties.report->AddMember("otherData", other_data_object, properties.report->GetAllocator());

				::rapidjson::Value trace_events_array;
				trace_events_array.SetArray();
				properties.report->AddMember("traceEvents", trace_events_array, properties.report->GetAllocator());
			}
		}

	public:
		static void Reset()
		{
			auto properties = _properties.get_access();
			Init(*properties);
			properties->report.reset();
			properties->isInitialized = false;
		}

		static void Save(const ::std::string filepath = "")
		{
			auto properties = _properties.get_access();
			Init(*properties);

			if (filepath.size() > 0)
				properties->filepath = filepath;

			SaveReport(*properties);
		}

		static void SetFilepath(const ::std::string filepath)
		{
			auto properties = _properties.get_access();
			Init(*properties);
			properties->filepath = filepath;
		}

		static ::std::string GetFilepath()
		{
			auto properties = _properties.get_access();
			Init(*properties);
			return properties->filepath;
		}

		static void EnableSaveOnTrace(bl enable = true)
		{
			auto properties = _properties.get_access();
			Init(*properties);
			properties->enableSaveOfTrace = enable;
		}

		static void EnableTraceAdd(bl enable = true)
		{
			auto properties = _properties.get_access();
			Init(*properties);
			properties->enableTrace = enable;
		}

		static void AddTraceEvent(TraceEvent& te)
		{
			auto properties = _properties.get_access();
			Init(*properties);

			if (properties->enableTrace)
			{
				::std::stringstream ss;
				ss << te.ThreadId;
				::std::string tid_str = ss.str();
				ss.str(""); // clear ss

				::rapidjson::MemoryPoolAllocator<::rapidjson::CrtAllocator>& allocator = properties->report->GetAllocator();
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
				trace.AddMember("ts", tim::DblMicroseconds(te.StartTimestamp.time_since_epoch()).count(), allocator);

				(*properties->report)["traceEvents"].PushBack(::std::move(trace), allocator);

				if (properties->enableSaveOfTrace)
					SaveReport(*properties);
			}
		}
	};
} // namespace np::nsit

#endif /* NP_ENGINE_INSTRUMENTOR_HPP */
