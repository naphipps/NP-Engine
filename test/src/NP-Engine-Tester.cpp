//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/2/21
//
//##===----------------------------------------------------------------------===##//

#include <exception>

#include "NP-Engine-Tester.hpp"

//TODO: I think our test app should change working dir
//TODO: I think our test app should contain all assets, including shaders
// TODO: move as much as we can into test proj

i32 main(i32 argc, chr** argv)
{
	::np::thr::ThisThread::SetAffinity(0);

	i32 exit_val = NP_ENGINE_EXIT_SUCCESS;
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
			::np::mem::sptr<::np::app::Application> application = ::np::mem::create_sptr<::np::app::GameApp>(services->GetAllocator(), services);
			application->Run(argc, argv);
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