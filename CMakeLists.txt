cmake_minimum_required(VERSION 3.20)
project(BerryFlux)

set(CMAKE_CXX_STANDARD 17)

# output folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Engine library
add_library(Engine SHARED
    Engine/Engine.cpp
)

target_include_directories(Engine PUBLIC Engine)

# Sandbox executable
add_executable(Sandbox
    Sandbox/main.cpp
)

target_link_libraries(Sandbox Engine)