//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/9/21
//
//##===----------------------------------------------------------------------===##//

// TODO: bring back xcode support - apparently our "fake" targets in cmake are bad for xcode

#include <exception>

#include "NP-Engine/NP-Engine.hpp"

extern ::np::app::Application* ::np::app::CreateApplication(::np::services::Services& application_services);

i32 main(i32 argc, chr** argv)
{
	::np::concurrency::ThisThread::SetAffinity(0);

	i32 retval = 0;
	str message;
	::np::app::Popup::Style style = ::np::app::Popup::DefaultStyle;
	::np::app::Popup::Buttons buttons = ::np::app::Popup::DefaultButtons;

	try
	{
		::np::system::Init(); // TODO: get a way to save the original working dir where the executable lives
		::np::memory::CAllocator& main_allocator = ::np::memory::DefaultAllocator;
		::np::memory::Block main_block = main_allocator.Allocate(NP_ENGINE_MAIN_MEMORY_SIZE);

		if (main_block.IsValid())
		{
			::np::memory::RedBlackTreeAllocator application_allocator(main_block);
			::np::memory::TraitAllocator::Register(application_allocator);
			::np::services::Services* application_services = ::np::memory::Create<::np::services::Services>(application_allocator);
			::np::app::Application* application = ::np::app::CreateApplication(*application_services);
			application->Run(argc, argv);
			::np::memory::Destroy<::np::app::Application>(application_allocator, application);
			::np::memory::Destroy<::np::services::Services>(application_allocator, application_services);
			NP_ENGINE_PROFILE_SAVE();
			NP_ENGINE_PROFILE_RESET();
			::np::memory::TraitAllocator::Register(::np::memory::DefaultAllocator);
		}
		else
		{
			retval = 3;
			message = "WAS NOT ABLE TO ALLOCATE ENOUGH MEMORY\n";
			message += "Log file can be found here: " + ::np::insight::Log::GetFileLoggerFilePath();
			style = ::np::app::Popup::Style::Error;
			NP_ENGINE_LOG_ERROR(message);
		}

		main_allocator.Deallocate(main_block);
	}
	catch (const ::std::exception& e)
	{
		::np::memory::TraitAllocator::Register(::np::memory::DefaultAllocator);
		retval = 1;
		message = "STD EXCEPTION OCCURRED: \n" + to_str(e.what()) + "\n\n";
		message += "Log file can be found here: " + ::np::insight::Log::GetFileLoggerFilePath();
		style = ::np::app::Popup::Style::Error;
		NP_ENGINE_LOG_ERROR(message);
	}
	catch (...)
	{
		::np::memory::TraitAllocator::Register(::np::memory::DefaultAllocator);
		retval = 2;
		message = "UNKNOWN EXCEPTION OCCURRED\n\n";
		message += "Log file can be found here: " + ::np::insight::Log::GetFileLoggerFilePath();
		style = ::np::app::Popup::Style::Error;
		NP_ENGINE_LOG_ERROR(message);
	}

	if (retval != 0)
	{
		::np::app::Popup::Show("NP-Engine Code: " + to_str(retval), message, style, buttons);
	}

	return retval; // TODO: use sysexits return values
}
