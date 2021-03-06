//===----------------------------------------------------------------------===//
//
//  ConstantsSettings.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 5/20/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//
//===----------------------------------------------------------------------===//

#ifndef NP_ENGINE_CONSTANTS_SETTINGS_HPP
#define NP_ENGINE_CONSTANTS_SETTINGS_HPP

//===----------------------------------------------------------------------===//

#define B2_USE_16_BIT_PARTICLE_INDICES

//===----------------------------------------------------------------------===//

#ifndef NP_ENGINE_MAIN_MEMORY_SIZE
#define NP_ENGINE_MAIN_MEMORY_SIZE (2000000000)
#endif /* NP_ENGINE_MAIN_MEMORY_SIZE */

//===----------------------------------------------------------------------===//

#if DEBUG

#define NP_PROFILE_ENABLE 1
#define NP_LOG_ENABLE 1
#define NP_ASSERT_ENABLE 1
#define OUTPUT_DIR "/Users/nathanphipps/Projects/Project Space/_output/" //TODO: remove this and get a better setup for this / logging in general

#else

#define OUTPUT_DIR "/Users/nathanphipps/Projects/Project Space/_output2/"

#endif

//===----------------------------------------------------------------------===//

#endif /* NP_ENGINE_CONSTANTS_SETTINGS_HPP */
