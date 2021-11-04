//
//  PlatformConstants.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/10/21.
//

#ifndef NP_ENGINE_PLATFORM_CONSTANTS_HPP
#define NP_ENGINE_PLATFORM_CONSTANTS_HPP

#if defined(__APPLE__) || defined(macintosh) || defined(Macintosh) || defined(__MACH__)
#define NP_ENGINE_PLATFORM_IS_APPLE true
#else
#define NP_ENGINE_PLATFORM_IS_APPLE false
#endif

#if defined(__WINDOWS__) || defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__)
#define NP_ENGINE_PLATFORM_IS_WINDOWS true
#else
#define NP_ENGINE_PLATFORM_IS_WINDOWS false
#endif

#if defined(__linux__) || defined(__linux) || defined(linux)
#define NP_ENGINE_PLATFORM_IS_LINUX true
#else
#define NP_ENGINE_PLATFORM_IS_LINUX false
#endif

//ensure only one platform is detected
#if NP_ENGINE_PLATFORM_IS_APPLE && NP_ENGINE_PLATFORM_IS_WINDOWS
#error NP detected Apple and Windows - only one is allowed
#elif NP_ENGINE_PLATFORM_IS_APPLE && NP_ENGINE_PLATFORM_IS_LINUX
#error NP detected Apple and Linux - only one is allowed
#elif NP_ENGINE_PLATFORM_IS_WINDOWS && NP_ENGINE_PLATFORM_IS_LINUX
#error NP detected Windows and Linux - only one is allowed
#endif

//perform platform checks
#if NP_ENGINE_PLATFORM_IS_WINDOWS
#if defined(_WIN32) && !defined(_WIN64)
#error NP detected x86 platform - NP does not support
#endif

#elif NP_ENGINE_PLATFORM_IS_APPLE
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULARTOR == 1
#error NP detected iPhone simulator - NP does not support
#elif TARGET_OS_IPHONE == 1
#error NP detected iPhone - NP does not support
#elif TARGET_OS_MAC == 1
#endif // TARGET_IPHONE_SIMULARTOR || TARGET_OS_IPHONE || TARGET_OS_MAC

#elif NP_ENGINE_PLATFORM_IS_LINUX
#warning NP detected linux - NP loosely supports

#else
#error NP detected unknown platform - NP does not support
#endif

#endif /* NP_ENGINE_PLATFORM_CONSTANTS_HPP */
