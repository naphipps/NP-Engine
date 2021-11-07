//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/9/21
//
//##===----------------------------------------------------------------------===##// //TODO: use these comment headers everywhere

#include <exception>

#include "NP-Engine/NP-Engine.hpp"

//TODO: is passing the main_block bellow enough?? I think passing the full allocator??
extern ::np::app::Application* ::np::app::CreateApplication(::np::memory::Allocator& application_allocator);

i32 main(i32 argc, chr** argv)
{
    ::np::system::Init();

    i32 retval = 0;
    ::np::memory::CAllocator& main_allocator = ::np::memory::DefaultAllocator;
    ::np::memory::Block main_block = main_allocator.Allocate(NP_ENGINE_MAIN_MEMORY_SIZE);
    
    if (main_block.IsValid())
    {
        try
        {
            //TODO: register application_allocator to TraitsAllocator
            ::np::memory::RedBlackTreeAllocator application_allocator(main_block);
            ::np::app::Application* app = ::np::app::CreateApplication(application_allocator);
            app->Run(argc, argv);
            ::np::memory::Destruct(app);
            application_allocator.Deallocate(app);
        }
        catch (const ::std::exception& e)
        {
            retval = 1;
            //TODO: how should we print this so the user knows??
            NP_LOG_FILE_ERROR("STD EXCEPTION OCCURRED: " + to_str(e.what()));
        }
        catch (...)
        {
            retval = 2;
            //TODO: how should we print this so the user knows??
            NP_LOG_FILE_ERROR("SOME OTHER EXCEPTION OCCURRED");
        }
    }
    else
    {
        retval = 3;
        //TODO: how should we print this so the user knows??
        NP_LOG_FILE_ERROR("WAS NOT ABLE TO ALLOCATE ENOUGH MEMORY");
    }
    
    main_allocator.Deallocate(main_block);
    return retval;
}
