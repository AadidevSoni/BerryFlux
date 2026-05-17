#pragma once
#include <memory>

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

#define BF_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace BerryFlux {

    template<typename T> //This is a template for a unique pointer. It can be used for any type T. It is a unique pointer that can only be owned by one object at a time. It is used for resources that are not shared between objects, such as a window or a renderer.
    using Scope = std::unique_ptr<T>;

    template<typename T>
    using Ref = std::shared_ptr<T>;
}