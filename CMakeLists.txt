cmake_minimum_required(VERSION 3.20)
project(BerryFlux)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# ---------------- Engine ----------------
# Builds BerryFlux as a shared library:
add_library(Engine SHARED
    BerryFlux/src/BerryFlux/Application.cpp
    BerryFlux/src/BerryFlux/Log.cpp
)

#Precompiled headers
target_precompile_headers(Engine PRIVATE BerryFlux/src/bfpch.h)

#PUBLIC means: Anyone linking this library gets these headers automatically So Sandbox inherits spdlog + engine headers.
target_include_directories(Engine PUBLIC
    BerryFlux/src
    BerryFlux/vendor/spdlog/include
)

if(APPLE)
    target_compile_definitions(Engine PRIVATE BF_PLATFORM_MACOS BF_BUILD_DLL)
endif()

target_compile_definitions(Engine PRIVATE
    $<$<CONFIG:Debug>:BF_DEBUG>
    $<$<CONFIG:Release>:BF_RELEASE>
)

target_compile_options(Engine PRIVATE
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O2>
)

# ---------------- Sandbox ----------------
add_executable(Sandbox
    Sandbox/src/SandboxApp.cpp
)

target_include_directories(Sandbox PRIVATE
    BerryFlux/src
    BerryFlux/vendor/spdlog/include
)

#Links engine to game.
target_link_libraries(Sandbox Engine)

#Ensures DLL sits next to executable. Otherwise OS can’t load it at runtime.
add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:Engine>
    $<TARGET_FILE_DIR:Sandbox>
)