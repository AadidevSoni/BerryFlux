# BerryFlux
### A Modern C++ Game Engine

BerryFlux is a **modular C++ game engine** currently under development for **3D real-time rendering applications**, games, simulations, and interactive experiences.

The goal of BerryFlux is to build a **data-driven, extensible, and cross-platform engine architecture** that separates platform code, rendering systems, and application logic.

---

# Overview

A **Game Engine** is a platform used to build **video games, VR experiences, and simulations**.

Unlike traditional applications that contain hardcoded logic, a game engine works by:

1. Reading **assets** (models, textures, shaders, scripts)
2. Processing and transforming this data
3. Rendering the final result to the screen
4. Handling interaction through **input, events, and systems**

In simple terms:

```
Assets → Engine Systems → Rendered Output
```

BerryFlux focuses on building the **core infrastructure** required for such a system.

---

# Architecture

The engine follows a **layered architecture** designed for modularity and portability.

```
Application (Client / Game)
        ↓
BerryFlux Engine
        ↓
Platform Layer
        ↓
Rendering API
        ↓
Operating System
```

Major systems in the engine include:

- Application Lifecycle
- Window Management
- Event System
- Input Handling
- Rendering Abstraction
- Logging System
- Layer System
- Platform Abstraction

Future planned systems:

- Entity Component System (ECS)
- Physics
- Memory Management
- Virtual File System (VFS)
- Asset Pipeline
- Scripting System

---

# Core Engine Systems

## Application Layer

The **Application** class acts as the **central hub of the engine**.

Responsibilities include:

- Managing the engine lifecycle
- Running the main loop
- Receiving and dispatching events
- Managing layers

Execution flow:

```
main()
   ↓
Log::Init()
   ↓
CreateApplication()  (defined by client)
   ↓
Application::Run()
   ↓
Main Engine Loop
```

---

# Entry Point System

BerryFlux controls the **entry point of the program**.

Instead of defining `main()` inside the client application, the engine defines it and expects the client to provide a factory function.

Example:

```cpp
Application* CreateApplication();
```

The client (Sandbox) implements this function.

This architecture allows the **engine to control startup and runtime flow**.

---

# Logging System

BerryFlux includes a **custom logging system** built on top of the `spdlog` library.

Features:

- Colored console logs
- Separate loggers for **engine and client**
- Macro-based logging for easier usage

Example:

```cpp
BF_CORE_INFO("Engine initialized");
BF_WARN("Sandbox started");
```

Library integration is done using **Git submodules**.

```
git submodule add https://github.com/gabime/spdlog BerryFlux/vendor/spdlog
```

---

# Event System

The event system allows communication between engine systems without tight coupling.

Examples of events:

- Window Resize
- Window Close
- Key Pressed
- Mouse Movement
- Mouse Scroll

Event hierarchy:

```
Event
 ├── KeyEvent
 │     ├── KeyPressedEvent
 │     └── KeyReleasedEvent
 ├── MouseMovedEvent
 ├── MouseScrolledEvent
 └── WindowResizeEvent
```

Events are categorized using **bit flags**.

Example:

```
EventCategoryInput       = 00010
EventCategoryMouse       = 01000
EventCategoryMouseButton = 10000
```

Multiple categories can be combined using bitwise OR.

```
category = EventCategoryInput | EventCategoryMouse
```

---

# Event Dispatcher

The **EventDispatcher** acts as a type-safe routing system for events.

Flow:

```
Event Occurs
     ↓
Event Dispatcher
     ↓
Checks Event Type
     ↓
Calls Correct Handler
```

This allows the engine to process events safely without excessive type checking.

---

# Window System

BerryFlux uses **platform abstraction** for window creation.

Engine code interacts with a **Window interface**, while the platform layer implements it.

Architecture:

```
Application
    ↓
Window (interface)
    ↓
GLFWWindow (implementation)
    ↓
GLFW
    ↓
Operating System
```

This design ensures the engine **does not depend directly on GLFW**.

---

# GLFW Integration

GLFW is used for:

- Window creation
- Input handling
- Event callbacks

GLFW is integrated as a **git submodule**.

```
git submodule add https://github.com/glfw/glfw BerryFlux/vendor/glfw
```

Callbacks from GLFW are converted into **BerryFlux events**.

Example flow:

```
User Resizes Window
        ↓
Operating System
        ↓
GLFW
        ↓
GLFW Callback
        ↓
WindowResizeEvent
        ↓
Application::OnEvent()
```

---

# Window Data System

GLFW callbacks are **C-style functions**, meaning they cannot access class members.

To solve this, BerryFlux stores runtime window state inside a structure:

```cpp
struct WindowData
```

This data is attached to the GLFW window:

```cpp
glfwSetWindowUserPointer(window, &m_Data);
```

Callbacks retrieve this data to interact with the engine.

---

# Precompiled Headers (PCH)

BerryFlux uses **Precompiled Headers** to improve compilation times.

Large headers such as:

- STL
- Logging libraries
- Template-heavy headers

are compiled once and reused.

Benefits:

- Faster build times
- Reduced header parsing

---

# Layer System

The **Layer system** allows modular stacking of engine functionality.

Layers can include:

- Game logic
- UI systems
- Rendering overlays
- Debug tools

Events propagate through the layer stack allowing systems to respond independently.

---

# Build System

BerryFlux uses **CMake** for cross-platform builds.

Build steps:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Run the Sandbox application:

```bash
./bin/Sandbox
```

---

# Project Structure

```
BerryFlux
│
├── BerryFlux
│   ├── src
│   │   ├── BerryFlux
│   │   │   ├── Core
│   │   │   ├── Events
│   │   │   ├── Window
│   │   │   └── Application
│
├── Sandbox
│   └── Example client application
│
├── vendor
│   ├── spdlog
│   └── glfw
│
├── build
└── CMakeLists.txt
```

---

# Current Features

- Engine controlled **Entry Point**
- **Application Lifecycle System**
- **Logging System (spdlog wrapper)**
- **Event System**
- **Event Dispatcher**
- **Window Abstraction**
- **GLFW Platform Implementation**
- **Precompiled Headers**
- **Layer Architecture**
- **CMake Build System**

---

# Planned Features

- OpenGL Rendering Pipeline
- Vulkan Rendering Backend
- Entity Component System (ECS)
- Physics Engine
- Asset Pipeline
- Virtual File System
- Scene System
- Editor UI
- Scripting Support

---

# Technologies Used

- **C++17**
- **CMake**
- **GLFW**
- **spdlog**
- **OpenGL (planned)**

---

# Author

**Aadidev Soni**

Game Engine Developer  
Computer Science Student
