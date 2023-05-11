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

::np::i32 main(::np::i32 argc, ::np::chr** argv)
{
	using namespace ::np;

	thr::ThisThread::SetAffinity(0);

	i32 exit_val = NP_ENGINE_EXIT_SUCCESS;
	str message;
	app::Popup::Style style = app::Popup::DefaultStyle;
	app::Popup::Buttons buttons = app::Popup::DefaultButtons;

	try
	{
		sys::Init();
		mem::AccumulatingAllocator<mem::RedBlackTreeAllocator> allocator;
		mem::TraitAllocator::Register(allocator);
		{
			NP_ENGINE_PROFILE_SCOPE("application lifespan");
			mem::sptr<srvc::Services> services = mem::create_sptr<srvc::Services>(allocator);
			mem::sptr<app::Application> application = mem::create_sptr<app::GameApp>(services->GetAllocator(), services);
			application->Run(argc, argv);
		}
		NP_ENGINE_PROFILE_SAVE();
		NP_ENGINE_PROFILE_RESET();
		mem::TraitAllocator::ResetRegistration();
	}
	catch (const ::std::exception& e)
	{
		mem::TraitAllocator::ResetRegistration();
		exit_val = NP_ENGINE_EXIT_STD_ERROR;
		message = "STD EXCEPTION OCCURRED: \n" + to_str(e.what()) + "\n\n";
		message += "Log file can be found here: " + nsit::Log::GetFileLoggerFilePath();
		style = app::Popup::Style::Error;
		NP_ENGINE_LOG_ERROR(message);
	}
	catch (...)
	{
		mem::TraitAllocator::ResetRegistration();
		exit_val = NP_ENGINE_EXIT_UNKNOWN_ERROR;
		message = "UNKNOWN EXCEPTION OCCURRED\n\n";
		message += "Log file can be found here: " + nsit::Log::GetFileLoggerFilePath();
		style = app::Popup::Style::Error;
		NP_ENGINE_LOG_ERROR(message);
	}

	if (exit_val != 0)
		app::Popup::Show("NP-Engine Exit Code: " + to_str(exit_val), message, style, buttons);

	return exit_val;
}