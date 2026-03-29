//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/19/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NSIT_INSTRUMENTOR_HPP
#define NP_ENGINE_NSIT_INSTRUMENTOR_HPP

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
	class instrumentor
	{
	public:
		struct properties
		{
			bl is_initialized = false;
			::std::shared_ptr<::rapidjson::Document> report = nullptr;
			::std::string filepath = "";
			bl enable_save_on_trace = false;
			bl enable_trace = true;
		};

	private:
		static mutexed_wrapper<properties> _properties;

		static void save_report(properties& p)
		{
			if (p.filepath.size() > 0)
			{
				log::get_logger()->info("Saving Instrumentor Profile Report...");

				::std::ofstream out_stream;
				out_stream.open(p.filepath);

				if (out_stream.is_open())
				{
					::rapidjson::StringBuffer buffer;
					::rapidjson::PrettyWriter<::rapidjson::StringBuffer> writer(buffer);
					p.report->Accept(writer);

					out_stream << buffer.GetString();
					out_stream.flush();
					out_stream.close();
				}

				log::get_logger()->info("Done Saving Instrumentor Profile Report: '" + p.filepath + "'");
			}
		}

		static void init(properties& p)
		{
			if (!p.is_initialized)
			{
				p.is_initialized = true;
				p.enable_trace = true;
				p.enable_save_on_trace = false;
				p.filepath = fsys::append(fsys::get_current_path(), "profile_report.json");

				p.report = ::std::make_shared<::rapidjson::Document>();
				p.report->SetObject();

				::rapidjson::Value other_data_object;
				other_data_object.SetObject();
				p.report->AddMember("otherData", other_data_object, p.report->GetAllocator());

				::rapidjson::Value trace_events_array;
				trace_events_array.SetArray();
				p.report->AddMember("traceEvents", trace_events_array, p.report->GetAllocator());
			}
		}

	public:
		static void reset()
		{
			auto p = _properties.get_access();
			init(*p);
			p->report.reset();
			p->is_initialized = false;
		}

		static void save(const ::std::string filepath = "")
		{
			auto p = _properties.get_access();
			init(*p);

			if (filepath.size() > 0)
				p->filepath = filepath;

			save_report(*p);
		}

		static void set_filepath(const ::std::string filepath)
		{
			auto p = _properties.get_access();
			init(*p);
			p->filepath = filepath;
		}

		static ::std::string get_filepath()
		{
			auto p = _properties.get_access();
			init(*p);
			return p->filepath;
		}

		static void enable_save_on_trace(bl enable = true)
		{
			auto p = _properties.get_access();
			init(*p);
			p->enable_save_on_trace = enable;
		}

		static void enable_trace_add(bl enable = true)
		{
			auto p = _properties.get_access();
			init(*p);
			p->enable_trace = enable;
		}

		static void add_trace_event(trace_event& e)
		{
			auto p = _properties.get_access();
			init(*p);

			if (p->enable_trace)
			{
				::std::stringstream ss;
				ss << e.thread_id;
				::std::string tid_str = ss.str();
				ss.str(""); // clear ss

				::rapidjson::MemoryPoolAllocator<::rapidjson::CrtAllocator>& allocator = p->report->GetAllocator();
				::rapidjson::Value name(e.name.c_str(), e.name.size(), allocator);
				::rapidjson::Value tid(tid_str.c_str(), tid_str.size(), allocator);
				::rapidjson::Value trace;

				trace.SetObject();
				trace.AddMember("cat", "function", allocator);
				trace.AddMember("dur", e.elapsed_microseconds.count(), allocator);
				trace.AddMember("name", name, allocator);
				trace.AddMember("ph", "X", allocator);
				trace.AddMember("pid", "0", allocator);
				trace.AddMember("tid", tid, allocator);
				trace.AddMember("ts", tim::microseconds(e.start_timestamp.time_since_epoch()).count(), allocator);

				(*p->report)["traceEvents"].PushBack(::std::move(trace), allocator);

				if (p->enable_save_on_trace)
					save_report(*p);
			}
		}
	};
} // namespace np::nsit

#endif /* NP_ENGINE_NSIT_INSTRUMENTOR_HPP */
