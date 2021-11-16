//
//  Assert.hpp
//  Project Space
//
//  Created by Nathan Phipps on 5/21/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_ASSERT_HPP
#define NP_ENGINE_ASSERT_HPP

#include <cassert>
#include <string>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Foundation/Foundation.hpp"

#include "Log.hpp"

namespace np
{
    namespace insight
    {
        /**
         this static assert method will be used as our assert function
         this forces a log to file and console
         */
        static void Assert(bl expression, ::std::string message)
        {
            if (!expression)
            {
                Log::GetLogger()->error(message);
                assert(expression); //will always fail
            }
        }
    }
}

#ifndef NP_ASSERT_ENABLE
#define NP_ASSERT_ENABLE 0
#endif

#if NP_ASSERT_ENABLE

#define NP_ASSERT(expression, message) ::np::insight::Assert(expression, message)
#define NP_STATIC_ASSERT(expression, message) static_assert(expression, message)

#else

#define NP_ASSERT(expression, message)
#define NP_STATIC_ASSERT(expression, message)

#endif //NP_ASSERT_ENABLE

#endif /* NP_ENGINE_ASSERT_HPP */
