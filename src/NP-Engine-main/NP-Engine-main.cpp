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

#include "NP-Engine/NP-Engine.hpp" //TODO: redesign how we want to do our search paths

//TODO: is passing the main_block bellow enough?? I think passing the full allocator??
extern ::np::app::Application* ::np::app::CreateApplication(const ::np::memory::Block& main_block);

i32 main(i32 argc, chr** argv)
{
    ::np::system::Init();
//     if (false)
//     {
//         using namespace ::np;
        
//         struct loud_destruct
//         {
//             void* ptrrrr = (void*)0x19;
            
//             ~loud_destruct()
//             {
//                 ::std::cout<<"goodbye cruel world!!\n";
//             }
//         };
        
//         memory::RedBlackTreeAllocator rbt(40000);

//         container::vector<loud_destruct> v(rbt);
//         container::vector<loud_destruct> b;

//         loud_destruct one;
//         one.ptrrrr = (void*)0x1;

//         v.push_back(one);
//         v.resize(5);
//         v.clear();
//         v.resize(6, loud_destruct());
        
//         b.swap(v);
        
//         return 0;
//     }
    
//     if (false)
//     {
//         ::std::vector<dbl> std_dbls{0.6, 0.7, 0.8};
//         auto std_begin = std_dbls.begin();
//         auto std_end = std_dbls.end();
//         auto std_rbegin = std_dbls.rbegin();
//         auto std_rend = std_dbls.rend();
        
// //        std_rbegin = std_end; // error
        
//         std_dbls.insert(std_dbls.begin(), std_dbls.rbegin(), std_dbls.rend());
        
        
//         using namespace ::np;
        
//         memory::RedBlackTreeAllocator rbt(30000);
        
//         container::vector<dbl> dbl1(rbt);
//         container::vector<dbl> dbl2(memory::DefaultAllocator);
        
//         dbl1 = {0.2, 0.3};
//         NP_ASSERT(dbl1.size() == 2, "");
//         NP_ASSERT(dbl1.capacity() == 4, "");
        
//         dbl1.insert(++dbl1.begin(), 0.25);
//         NP_ASSERT(dbl1.size() == 3, "");
//         NP_ASSERT(dbl1.capacity() == 4, "");
        
//         dbl2 = {0.2, 0.3, 0.4, 0.5, 0.6};
//         dbl2.pop_back();
//         dbl2.pop_back();
//         NP_ASSERT(dbl2.size() == 3, "");
//         NP_ASSERT(dbl2.capacity() == 8, "");
//         dbl2.shrink_to_fit();
//         NP_ASSERT(dbl2.size() == 3, "");
//         NP_ASSERT(dbl2.capacity() == 4, "");
        
//         dbl1.insert(dbl1.end(), dbl2.begin(), dbl2.end());
//         NP_ASSERT(dbl1.size() == 6, "");
//         NP_ASSERT(dbl1.capacity() == 8, "");
        
//         dbl1.insert(dbl1.begin(), dbl1.front());
//         NP_ASSERT(dbl1.size() == 7, "");
//         NP_ASSERT(dbl1.capacity() == 8, "");
        
//         dbl1.insert(dbl1.end(), 3, dbl2.back());
//         NP_ASSERT(dbl1.size() == 10, "");
//         NP_ASSERT(dbl1.capacity() == 16, "");
        
//         dbl1.insert(dbl1.begin() + 2, {0.1, 0.2, 0.3});
//         NP_ASSERT(dbl1.size() == 13, "");
//         NP_ASSERT(dbl1.capacity() == 16, "");
        
//         dbl1.insert(dbl1.end(), dbl1.rbegin(), dbl1.rend());
//         NP_ASSERT(dbl1.size() == 26, "");
//         NP_ASSERT(dbl1.capacity() == 32, "");
        
//         dbl1.erase(dbl1.end());
//         NP_ASSERT(dbl1.size() == 26, "");
//         NP_ASSERT(dbl1.capacity() == 32, "");
        
//         dbl1.erase(dbl1.end() - 1);
//         NP_ASSERT(dbl1.size() == 25, "");
//         NP_ASSERT(dbl1.capacity() == 32, "");
        
//         dbl1.erase(dbl1.begin(), dbl1.begin() + 1);
//         NP_ASSERT(dbl1.size() == 24, "");
//         NP_ASSERT(dbl1.capacity() == 32, "");
        
//         dbl1.assign(10, 0.3);
//         NP_ASSERT(dbl1.size() == 10, "");
//         NP_ASSERT(dbl1.capacity() == 16, "");
        
//         dbl1.assign({0.9, 0.8, 0.7});
//         NP_ASSERT(dbl1.size() == 3, "");
//         NP_ASSERT(dbl1.capacity() == 4, "");
        
//         dbl1.assign(dbl2.rbegin(), dbl2.rend());
//         NP_ASSERT(dbl1.size() == dbl2.size(), "");
//         NP_ASSERT(dbl1.capacity() == 4, "");
        
//         dbl1.at(0);
//         dbl1.clear();
//         NP_ASSERT(dbl1.size() == 0, "");
//         NP_ASSERT(dbl1.capacity() == 4, "");
        
//         dbl1 = {0.1, 0.2, 0.3};
//         dbl1.back();
//         dbl1.begin();
//         dbl1.end();
//         dbl1.rbegin();
//         dbl1.rend();
//         dbl1.cbegin();
//         dbl1.cend();
//         dbl1.crbegin();
//         dbl1.crend();
//         dbl1.push_back(0.4);
//         dbl1.push_back(0.3);
//         dbl1.pop_back();
//         dbl1.emplace(dbl1.end(), 0.5);
//         dbl1.emplace_back(0.6);
// //        dbl1 = dbl2;
//         dbl1.swap(dbl2);
        
        
//         return 0;
//     }
    
//     if (true)
//     {
//         ::np::container::deque<ui16> int16s;
        
        
//         return 0;
//     }
    
    i32 retval = 0;
    ::np::memory::CAllocator& main_allocator = ::np::memory::DefaultAllocator;
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
