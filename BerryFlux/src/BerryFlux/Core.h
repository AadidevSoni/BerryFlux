#pragma once

#if defined(_WIN32) || defined(_WIN64)

    #ifdef BF_BUILD_DLL
        #define BERRYFLUX_API __declspec(dllexport)
    #else
        #define BERRYFLUX_API __declspec(dllimport)
    #endif

#else
    // macOS / Linux → no export keyword needed
    #define BERRYFLUX_API
#endif