cmake_minimum_required(VERSION 3.20)
project(BerryFlux)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# ---------------- GLFW aand GLAD ----------------
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(BerryFlux/vendor/GLFW) #add as subdirectory only if it has its own CMake
add_subdirectory(BerryFlux/vendor/GLAD)
add_subdirectory(BerryFlux/vendor/imgui)

# ---------------- Engine ----------------
# Builds BerryFlux as a shared library:
add_library(Engine SHARED
    BerryFlux/src/BerryFlux/Application.cpp
    BerryFlux/src/BerryFlux/Log.cpp
    BerryFlux/src/Platform/GLFWWindow.cpp
    BerryFlux/src/BerryFlux/Layer.cpp
    BerryFlux/src/BerryFlux/LayerStack.cpp
    BerryFlux/src/BerryFlux/ImGui/ImGuiLayer.cpp
    BerryFlux/src/Platform/WindowsInput.cpp
)

#Precompiled headers
target_precompile_headers(Engine PRIVATE BerryFlux/src/bfpch.h)

#PUBLIC means: Anyone linking this library gets these headers automatically So Sandbox inherits spdlog + engine headers.
target_include_directories(Engine PUBLIC
    BerryFlux/src
    BerryFlux/vendor/spdlog/include
    BerryFlux/vendor/glad/include
)

#Link GLFW to Engine
target_link_libraries(Engine PRIVATE glfw glad ImGui)

if(APPLE)
    target_compile_definitions(Engine PRIVATE BF_PLATFORM_MACOS BF_BUILD_DLL)
    find_library(OPENGL_LIBRARY OpenGL)
    target_link_libraries(Engine PRIVATE ${OPENGL_LIBRARY})
endif()

target_compile_definitions(Engine PRIVATE
    $<$<CONFIG:Debug>:BF_DEBUG>
    $<$<CONFIG:Release>:BF_RELEASE>
    $<$<CONFIG:Debug>:BF_ENABLE_ASSERTS>
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