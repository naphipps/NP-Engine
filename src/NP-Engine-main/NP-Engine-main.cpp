//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/9/21
//
//##===----------------------------------------------------------------------===##// //TODO: use these comment headers everywhere

#include <exception>

#include "NP-Engine/NP-Engine.hpp"

extern ::np::app::Application* ::np::app::CreateApplication(::np::memory::Allocator& application_allocator);

i32 main(i32 argc, chr** argv)
{
    ::np::system::Init(); //TODO: register TraitAllocator here maybe??

    i32 retval = 0;
    str message;
    ::np::system::Popup::Style style = ::np::system::Popup::DefaultStyle;
    ::np::system::Popup::Buttons buttons = ::np::system::Popup::DefaultButtons;

    ::np::memory::CAllocator& main_allocator = ::np::memory::DefaultAllocator;
    ::np::memory::Block main_block = main_allocator.Allocate(NP_ENGINE_MAIN_MEMORY_SIZE);
    
    if (main_block.IsValid())
    {
        try
        {
            ::np::memory::RedBlackTreeAllocator application_allocator(main_block);
            ::np::app::Application* app = ::np::app::CreateApplication(application_allocator);
            app->Run(argc, argv);
            ::np::memory::Destruct(app);
            application_allocator.Deallocate(app);
        }
        catch (const ::std::exception& e)
        {
            retval = 1;
            message = "STD EXCEPTION OCCURRED: " + to_str(e.what()) + "\n";
            message += "Log file can be found here: " + ::np::insight::Log::GetFileLoggerFilePath();
            style = ::np::system::Popup::Style::Error;
            NP_LOG_FILE_ERROR(message);
        }
        catch (...)
        {
            retval = 2;
            message = "SOME OTHER EXCEPTION OCCURRED\n";
            message += "Log file can be found here: " + ::np::insight::Log::GetFileLoggerFilePath();
            style = ::np::system::Popup::Style::Error;
            NP_LOG_FILE_ERROR(message);
        }
    }
    else
    {
        retval = 3;
        message = "WAS NOT ABLE TO ALLOCATE ENOUGH MEMORY\n";
        message += "Log file can be found here: " + ::np::insight::Log::GetFileLoggerFilePath();
        style = ::np::system::Popup::Style::Error;
        NP_LOG_FILE_ERROR(message);
    }

    if (retval != 0)
    {
        ::np::system::Popup::Show("NP-Engine Code: " + retval, message, style, buttons);
    }
    
    main_allocator.Deallocate(main_block);
    return retval;
}
