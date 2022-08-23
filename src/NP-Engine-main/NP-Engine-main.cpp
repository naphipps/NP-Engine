//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/9/21
//
//##===----------------------------------------------------------------------===##//

// TODO: bring back xcode support - apparently our "fake" targets in cmake are bad for xcode

#include <exception>

#include "NP-Engine/NP-Engine.hpp"

extern ::np::app::Application* ::np::app::CreateApplication(::np::srvc::Services& app_services);

i32 main(i32 argc, chr** argv)
{
	{
		using namespace np;

		con::array<::glm::vec2, 10> poly;
		::glm::vec2 point;

		bl is = alg::pnpoly(poly, point);

		con::vector<::glm::vec2> cir = alg::GetMidpointCirclePoints<::glm::vec2::value_type>(30);

		con::array<::glm::vec2, 2000> points;
		siz points_size;
		::glm::vec2 offset;

		bl b = alg::GetMidpointCirclePoints(30, points, points_size, offset);

		::glm::vec2 lb, le;
		con::vector<::glm::vec2> line = alg::GetBresenhamLinePoints(lb, le);

		con::array<::glm::vec2, 3000> line2;
		siz line_size;
		bl line_yes = alg::GetBresenhamLinePoints(line2, line_size, lb, le);

		::glm::vec2 p, q;
		alg::ClipLineReturn clr = alg::ClipLine<::glm::vec2::value_type>(p, q, 4, 5);

		return 0;
	}

	::np::thr::ThisThread::SetAffinity(0);

	i32 retval = 0;
	str message;
	::np::app::Popup::Style style = ::np::app::Popup::DefaultStyle;
	::np::app::Popup::Buttons buttons = ::np::app::Popup::DefaultButtons;

	try
	{
		::np::sys::Init(); // TODO: get a way to save the original working dir where the executable lives
		::np::mem::CAllocator main_allocator;
		::np::mem::Block main_block = main_allocator.Allocate(NP_ENGINE_MAIN_MEMORY_SIZE);

		if (main_block.IsValid())
		{
			::np::mem::RedBlackTreeAllocator app_allocator(main_block);
			::np::mem::TraitAllocator::Register(app_allocator);
			::np::srvc::Services* app_services = ::np::mem::Create<::np::srvc::Services>(app_allocator);
			::np::app::Application* application = ::np::app::CreateApplication(*app_services);
			application->Run(argc, argv);
			::np::mem::Destroy<::np::app::Application>(app_allocator, application);
			::np::mem::Destroy<::np::srvc::Services>(app_allocator, app_services);
			NP_ENGINE_PROFILE_SAVE();
			NP_ENGINE_PROFILE_RESET();
			::np::mem::TraitAllocator::ResetRegistration();
			main_allocator.Deallocate(main_block);
		}
		else
		{
			::np::mem::TraitAllocator::ResetRegistration();
			retval = 3;
			message = "WAS NOT ABLE TO ALLOCATE ENOUGH MEMORY\n";
			message += "Log file can be found here: " + ::np::nsit::Log::GetFileLoggerFilePath();
			style = ::np::app::Popup::Style::Error;
			NP_ENGINE_LOG_ERROR(message);
		}
	}
	catch (const ::std::exception& e)
	{
		::np::mem::TraitAllocator::ResetRegistration();
		retval = 1;
		message = "STD EXCEPTION OCCURRED: \n" + to_str(e.what()) + "\n\n";
		message += "Log file can be found here: " + ::np::nsit::Log::GetFileLoggerFilePath();
		style = ::np::app::Popup::Style::Error;
		NP_ENGINE_LOG_ERROR(message);
	}
	catch (...)
	{
		::np::mem::TraitAllocator::ResetRegistration();
		retval = 2;
		message = "UNKNOWN EXCEPTION OCCURRED\n\n";
		message += "Log file can be found here: " + ::np::nsit::Log::GetFileLoggerFilePath();
		style = ::np::app::Popup::Style::Error;
		NP_ENGINE_LOG_ERROR(message);
	}

	if (retval != 0)
	{
		::np::app::Popup::Show("NP-Engine Code: " + to_str(retval), message, style, buttons);
	}

	return retval; // TODO: use sysexits return values
}
