cmake_minimum_required(VERSION 3.20)
project(BerryFlux)

set(CMAKE_CXX_STANDARD 17)

# output folders
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ---------------- Engine ----------------
add_library(Engine SHARED
    BerryFlux/src/BerryFlux/Application.cpp
)

target_include_directories(Engine PUBLIC
    BerryFlux/src
)

# ---------------- Sandbox ----------------
add_executable(Sandbox
    Sandbox/src/SandboxApp.cpp
)

target_link_libraries(Sandbox Engine)