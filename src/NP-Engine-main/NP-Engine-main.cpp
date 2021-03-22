//===----------------------------------------------------------------------===//
//
//  NP-Engine-main.cpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/9/21.
//  Copyright © 2021 Nathan Phipps. All rights reserved.
//
//===----------------------------------------------------------------------===//

#include <exception>

#include "../NP-Engine.hpp" //TODO: redesign how we want to do our search paths

//TODO: is passing the main_block bellow enough?? I think passing the full allocator??
extern ::np::app::Application* ::np::app::CreateApplication(const ::np::memory::Block& main_block);

i32 main(i32 argc, chr** argv)
{
    i32 retval = 0;
    ::np::memory::CAllocator main_allocator;
    ::np::memory::Block main_block = main_allocator.Allocate(NP_ENGINE_MAIN_MEMORY_SIZE);
    
    if (main_block.IsValid())
    {
        try
        {
            ::np::app::Application* app = ::np::app::CreateApplication(main_block);
            app->Run();
            ::np::memory::Destruct(app);
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
