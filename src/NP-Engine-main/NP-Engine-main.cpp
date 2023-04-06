//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/9/21
//
//##===----------------------------------------------------------------------===##//

// TODO: bring back xcode support - apparently our "fake" targets in cmake are bad for xcode
// TODO: all "::Create" functions need to return a sptr

#include <exception>

#include "NP-Engine/NP-Engine.hpp"

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
		::np::mem::AccumulatingAllocator<::np::mem::RedBlackTreeAllocator> allocator;
		::np::mem::TraitAllocator::Register(allocator);
		{
			NP_ENGINE_PROFILE_SCOPE("application lifespan");
			::np::mem::sptr<::np::srvc::Services> services = ::np::mem::create_sptr<::np::srvc::Services>(allocator);
			::np::mem::sptr<::np::app::Application> application = ::np::app::CreateApplication(*services);
			application->Run(argc, argv);
			application.reset(); //TODO: when services is treated as sptr in application, then we can remove these resets
			services.reset();
		}
		NP_ENGINE_PROFILE_SAVE();
		NP_ENGINE_PROFILE_RESET();
		::np::mem::TraitAllocator::ResetRegistration();
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
