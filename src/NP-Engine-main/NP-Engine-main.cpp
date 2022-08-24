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
	::np::thr::ThisThread::SetAffinity(0);

	siz exit_val = NP_ENGINE_EXIT_SUCCESS;
	str message;
	::np::app::Popup::Style style = ::np::app::Popup::DefaultStyle;
	::np::app::Popup::Buttons buttons = ::np::app::Popup::DefaultButtons;

	try
	{
		::np::sys::Init();
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
			exit_val = NP_ENGINE_EXIT_MEMORY_ERROR;
			message = "WAS NOT ABLE TO ALLOCATE ENOUGH MEMORY\n";
			message += "Log file can be found here: " + ::np::nsit::Log::GetFileLoggerFilePath();
			style = ::np::app::Popup::Style::Error;
			NP_ENGINE_LOG_ERROR(message);
		}
	}
	catch (const ::std::exception& e)
	{
		::np::mem::TraitAllocator::ResetRegistration();
		exit_val = NP_ENGINE_EXIT_STD_ERROR;
		message = "STD EXCEPTION OCCURRED: \n" + to_str(e.what()) + "\n\n";
		message += "Log file can be found here: " + ::np::nsit::Log::GetFileLoggerFilePath();
		style = ::np::app::Popup::Style::Error;
		NP_ENGINE_LOG_ERROR(message);
	}
	catch (...)
	{
		::np::mem::TraitAllocator::ResetRegistration();
		exit_val = NP_ENGINE_EXIT_UNKNOWN_ERROR;
		message = "UNKNOWN EXCEPTION OCCURRED\n\n";
		message += "Log file can be found here: " + ::np::nsit::Log::GetFileLoggerFilePath();
		style = ::np::app::Popup::Style::Error;
		NP_ENGINE_LOG_ERROR(message);
	}

	if (exit_val != 0)
		::np::app::Popup::Show("NP-Engine Exit Code: " + to_str(exit_val), message, style, buttons);

	return exit_val;
}
