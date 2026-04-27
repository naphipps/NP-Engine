//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/2/21
//
//##===----------------------------------------------------------------------===##//

#include <exception>

#include "NP-Engine-Tester.hpp"

// TODO: I think our test app should change working dir
// TODO: I think our test app should contain all assets, including shaders
// TODO: move as much as we can into test proj

::np::i32 main(::np::i32 argc, ::np::chr** argv)
{
	using namespace ::np;

	thr::this_thread::set_affinity(0);

	i32 exit_val = NP_ENGINE_EXIT_SUCCESS;
	str message;
	win::PopupStyle style = win::PopupStyle::Info;
	win::PopupButtons buttons = win::PopupButtons::Ok;

	try
	{
		sys::init();
		mem::accumulating_allocator<mem::red_black_tree_allocator> allocator{};
		mem::trait_allocator::register_allocator(allocator);
		{
			NP_ENGINE_PROFILE_SCOPE("application lifespan");
			mem::sptr<srvc::Services> services = mem::create_sptr<srvc::Services>(allocator);
			mem::sptr<app::Application> application = mem::create_sptr<app::GameApp>(services->GetAllocator(), services);
			application->Run(argc, argv);
		}
		NP_ENGINE_PROFILE_SAVE();
		NP_ENGINE_PROFILE_RESET();
		mem::trait_allocator::reset_registration();
	}
	catch (const ::std::exception& e)
	{
		mem::trait_allocator::reset_registration();
		exit_val = NP_ENGINE_EXIT_STD_ERROR;
		message = "STD EXCEPTION OCCURRED: \n" + to_str(e.what()) + "\n\n";
		message += "Log file can be found here: " + nsit::log::get_file_logger_file_path();
		style = win::PopupStyle::Error;
		NP_ENGINE_LOG_ERROR(message);
	}
	catch (...)
	{
		mem::trait_allocator::reset_registration();
		exit_val = NP_ENGINE_EXIT_UNKNOWN_ERROR;
		message = "UNKNOWN EXCEPTION OCCURRED\n\n";
		message += "Log file can be found here: " + nsit::log::get_file_logger_file_path();
		style = win::PopupStyle::Error;
		NP_ENGINE_LOG_ERROR(message);
	}

	if (exit_val != 0)
		win::Popup::Show(nullptr, "NP-Engine Exit Code: " + to_str(exit_val), message, style, buttons);

	return exit_val;
}