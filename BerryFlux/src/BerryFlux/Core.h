#pragma once

#if defined(_WIN32) || defined(_WIN64)

    #ifdef BF_BUILD_DLL
        #define BERRYFLUX_API __declspec(dllexport)
    #else
        #define BERRYFLUX_API __declspec(dllimport)
    #endif

#else
    // macOS / Linux → no export keyword needed
    #define BERRYFLUX_API //This makes the engine usable as a shared library.
#endif

#ifdef BF_ENABLE_ASSERTS
    #define BF_ASSERT(x, ...) {if(!(x)) {BF_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
    #define BF_CORE_ASSERT(x, ...) {if(!(x)) {BF_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
#else 
    #define BF_ASSERT(x, ...)
    #define BF_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)