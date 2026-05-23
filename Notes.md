# BerryFlux Game Engine — Development Notes
*Following TheCherno's Hazel Engine Series | C++ & OpenGL*

---

## What Is a Game Engine?

A game engine is a **platform for building video games, VR experiences, and simulations**. At its core, it is a data transformation and interactivity system — it reads asset files, processes them through various systems, and outputs them to the screen.

- It is **not** a hardcoded application. An application has baked-in behavior; a game engine is general-purpose.
- **Input:** Asset files (3D models, textures, scripts, audio, etc.)
- **Output:** Rendered frames on screen
- Key subsystems handle platform abstraction so the same engine code runs on Windows, Mac, Linux, etc.

---

## Engine Architecture Plan

Before writing any code, the engine was designed around these layers and systems:

| System | Purpose |
|---|---|
| **Entry Point** | Controls what happens when the engine launches |
| **Application Layer** | Lifecycle events, run loops, timing, window resize/close |
| **Window Layer** | Platform window creation and management |
| **Input** | Tracks input state — keys held/pressed, mouse position |
| **Event System** | Messaging and broadcast system for all engine events |
| **Renderer** | Renders graphics to screen |
| **Render API Abstraction** | Supports multiple rendering APIs (starts with OpenGL) |
| **Debugging / Profiling** | Console logging, performance analysis |
| **Scripting Language** | Exposes engine functionality to game scripts |
| **Memory Systems** | Custom allocators and memory tracking |
| **Entity Component System (ECS)** | Creates game objects and attaches behavior components |
| **Physics** | Simulation of physical interactions |
| **File I/O & Virtual File System** | Reading/writing assets; path abstraction |
| **Build System** | Converts third-party 3D model formats into engine-native formats at build time |

---

## Project Setup

### Repository and Solution Structure

1. Create a Visual Studio solution.
2. Clone the GitHub repository into the project folder.
3. Copy all required files into the project folder (avoid committing the initial empty state of folders).

### Engine as a DLL (Dynamic Link Library)

- The engine (`BerryFlux`) is built as a **DLL** (`.dylib` on macOS).
- **Why DLL instead of static library?** With static linking, every third-party library (spdlog, GLFW, etc.) would need to be linked directly into the game executable. Instead, we link all static libraries into the engine DLL once, and the game only depends on that single DLL.
- Create a `Sandbox` project inside the solution — this is the game-side executable that consumes the engine.
- Set `Sandbox` as the **startup project**.
- Add a reference to `BerryFlux` in `Sandbox` to link the engine.

### CMake Build

The project is defined with CMake instead of raw VS project files, so it can generate platform-specific build systems (VS, Makefiles, Xcode, etc.).

```bash
cd build
cmake --build .
./bin/Sandbox
```

---

## Entry Point

**Goal:** Remove the `main()` function from the game (Sandbox). Define `main()` inside the engine so every game automatically gets a standard entry point.

### File Structure

```
BerryFlux/
  src/
    BerryFlux/        ← Core engine code
      Core.h          ← Engine API macros
      Application.h
    BerryFlux.h       ← Master include — Sandbox only needs this one header
    EntryPoint.h      ← Contains main(), creates and runs the Application
```

### Flow

- `EntryPoint.h` contains `main()` and is included by `Sandbox`.
- `main()` calls `CreateApplication()`, which is **defined by Sandbox** (not the engine).
- This creates a `Sandbox` instance (which extends `Application`) and calls `Application::Run()`.

### Program Execution Flow

```
main()              ← EntryPoint.cpp
  ↓
Log::Init()
  ↓
CreateApplication() ← defined by Sandbox
  ↓
new Sandbox()
  ↓
Application::Run()
  ↓
[infinite loop]
```

### Runtime Relationship

```
Sandbox executable
  └── loads BerryFlux.dll
        └── engine calls CreateApplication()
              └── Sandbox returns its Application subclass
```

---

## Logging

**Goal:** Log colored, tagged messages to the console so we can track what happens when the engine starts (file loading, shader initialization, etc.).

### Library: spdlog

- Added as a **git submodule** (not copied manually):

```bash
git submodule add https://github.com/gabime/spdlog BerryFlux/vendor/spdlog
```

- spdlog can format and print characters, objects, pointers, numbers, and more.

### Wrapper API

- Created `Log.h` and `Log.cpp` to wrap spdlog behind the engine's own API.
- Two separate loggers: one for the **engine** and one for the **client** (game).
- Each logger has its own formatting style.

### Macros

- At the end of `Log.h`, macros are defined that call the engine/client loggers.
- This means game code never directly calls spdlog — it calls `BF_CORE_WARN(...)`, `BF_INFO(...)`, etc.
- Initialized in `Log::Init()`, called from `EntryPoint.h` before `CreateApplication()`.

---

## Event System

**Goal:** Detect and respond to window, keyboard, and mouse events without tightly coupling the `Window` and `Application` classes.

### Design

- `Window` receives raw OS/GLFW events.
- `Window` does **not** know about `Application`.
- `Application` creates the `Window` and registers an **event callback** with it.
- When an event fires, `Window` calls the callback, which routes the event to `Application::OnEvent()`.
- This pattern is called a **blocking event** — events are dispatched immediately.

### Event Categories (Bitmask)

Categories are stored as bit flags so a single event can belong to multiple categories simultaneously:

```
EventCategoryInput       = 0b00010
EventCategoryMouse       = 0b01000
EventCategoryMouseButton = 0b10000

// Combined:
category = EventCategoryInput | EventCategoryMouse; // = 0b01010
```

This lets you check `event.IsInCategory(EventCategoryInput)` without a chain of if-else comparisons.

### Event Hierarchy

```
Event
 ├── KeyEvent
 │     ├── KeyPressedEvent
 │     └── KeyReleasedEvent
 ├── MouseMovedEvent
 ├── MouseScrolledEvent
 └── WindowResizeEvent
     WindowCloseEvent
```

### Files Created

- `Event.h` — base `Event` class, `EventType` enum, `EventCategory` enum (bitmask).
- `KeyEvent.h` — `KeyEvent`, `KeyPressedEvent`, `KeyReleasedEvent`.
- Macros in each event class handle the boilerplate for `GetEventType()` and `GetName()`.

### EventDispatcher

The `EventDispatcher` is a helper that:
1. Takes a generic `Event&`.
2. Checks its actual runtime type.
3. Calls the correct typed handler function safely (without a manual cast).

```
EventDispatcher
  ↓  checks type
  ↓  calls matching handler if types match
```

---

## Precompiled Headers (PCH)

A **Precompiled Header** is a header file that is compiled once and cached by the compiler. Every `.cpp` file that includes standard or large headers (like STL containers) forces re-parsing of thousands of lines — PCH eliminates that overhead.

Typical PCH candidates: `<iostream>`, `<string>`, `<vector>`, `<unordered_map>`, `<functional>`, etc. — headers that are:
- Huge
- Template-heavy
- Included in almost every translation unit

---

## Window Abstraction and GLFW

**Goal:** Decouple the engine from GLFW. The engine should work with any windowing library.

### Dependency Setup

- GLFW added as a git submodule with its own `CMakeLists.txt`, linked into the engine's CMake build.

### Architecture

```
Application
  ↓
Window (abstract interface — Window.h)
  ↓
GLFWWindow (concrete implementation — Platform/Windows/WindowsWindow.cpp)
  ↓
GLFW
  ↓
Operating System
```

- `Window.h` defines the interface (what a window must be able to do), not how it does it.
- `GLFWWindow` implements the interface using GLFW.
- `Application` only ever sees `Window*`. It never touches GLFW directly.
- `Window::Create(props)` is a **factory method** — the engine asks for a Window and the platform layer decides to create a `GLFWWindow`.

### WindowData Struct

GLFW callbacks are C-style functions — they cannot access class member variables. So a `WindowData` struct stores the runtime window state (width, height, event callback, etc.).

```cpp
glfwSetWindowUserPointer(m_Window, &m_Data);
// Later inside callbacks:
auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
```

### Application Integration

- `Application` holds a `Window*` (or `Scope<Window>`).
- Calls `m_Window->OnUpdate()` inside the `Run()` loop, which polls events and swaps buffers.

---

## Window Events

**Full event path for a window resize:**

```
User resizes window
  ↓ OS
  ↓ GLFW
  ↓ glfwSetWindowSizeCallback
  ↓ Lambda in GLFWWindow.cpp
  ↓ WindowResizeEvent created
  ↓ EventCallback(event)         ← registered by Application
  ↓ Application::OnEvent(event)
```

### Steps

1. Implemented all GLFW event callbacks in `GLFWWindow.cpp` (resize, close, key, mouse, scroll).
2. Each callback creates the appropriate `BerryFlux::Event` subclass and calls the registered `EventCallback`.
3. In `Application::OnEvent()`, if the event is a `WindowCloseEvent`, set `m_Running = false` to exit the loop.

---

## Layer System

**Goal:** Separate engine systems into independent, stackable modules. Avoid dumping everything into `Application`.

### Concept

Layers are like transparent sheets stacked on top of each other. Each layer can:
- Update every frame (`OnUpdate`)
- Receive and consume events (`OnEvent`)
- Render content (`OnRender`)
- Be pushed and popped dynamically

### Layer Stack

```
Top (rendered last, receives events first)
│  ImGui Layer    (UI / Debug — Overlay)
│  Game Layer     (Gameplay Logic)
│  Engine Systems
Bottom (rendered first)
```

### Event Propagation

Events travel **top-to-bottom** through the stack (overlays first):

```
Keyboard Event
  ↓ Application
  ↓ ImGuiLayer       ← can consume it here
  ↓ GameLayer
  ↓ EngineLayer
```

If a layer sets `event.m_Handled = true`, propagation stops. This allows UI to "eat" input before the game sees it.

### Why Iterate Backwards for Events?

The internal vector stores layers `[GameLayer, PhysicsLayer, UILayer, ImGuiLayer, DebugOverlay]`. But visually, `DebugOverlay` is on top. Iterating in **reverse** gives top-of-screen layers priority on events.

### Overlay Layers

UI and debug layers (like ImGui) that sit above game layers. Pushed via `PushOverlay()` instead of `PushLayer()` so they always stay on top.

### Files Created

- `Layer.h` / `Layer.cpp` — base `Layer` class with `OnAttach`, `OnDetach`, `OnUpdate`, `OnEvent`.
- `LayerStack.h` / `LayerStack.cpp` — manages the vector of layers; layers inserted before overlays, overlays always at the end.
- `Application.cpp` updated with `PushLayer()` / `PushOverlay()`, and in `Run()`, loops through layers calling `OnUpdate()` and `OnEvent()`.
- Created an `ExampleLayer` in Sandbox to test the system.

---

## Modern OpenGL with GLAD

- **GLAD** replaces GLEW as the OpenGL function loader.
- Its job: locate OpenGL function pointers in the GPU driver DLL at runtime and expose them as callable C++ functions.
- **Do not** include any raw OpenGL headers directly; use GLAD's header with a simple macro guard.
- Initialized in `GLFWWindow.cpp` after the GLFW context is created (`gladLoadGLLoader`).

### Engine Module Ownership

```
BerryFlux
├── Core
│   ├── Application
│   └── Layer
├── Platform
│   └── GLFWWindow          ← only this knows about GLFW
└── Renderer
    ├── Renderer
    ├── RendererAPI
    └── OpenGLRendererAPI   ← only this knows about OpenGL
```

---

## ImGui Integration

**Goal:** Add an immediate-mode debug UI layer on top of the engine.

### Temporary Approach

For now, the ImGui layer calls raw OpenGL functions directly. Later, it will go through the BerryFlux Renderer API.

### Setup

- Added ImGui as a submodule.
- Created `ImGuiLayer` which extends `Layer`.
- In `OnAttach()`: called `ImGui_ImplGlfw_InitForOpenGL(window, true)`.
  - This backend automatically registers GLFW callbacks for keyboard, mouse, scroll, and char input — ImGui gets input automatically without us wiring it up.
- In `OnUpdate()` (the render loop):
  1. `ImGui_ImplOpenGL3_NewFrame()`
  2. `ImGui_ImplGlfw_NewFrame()`
  3. `ImGui::NewFrame()`
  4. Render ImGui widgets
  5. `ImGui::Render()` + `ImGui_ImplOpenGL3_RenderDrawData()`

### Application Integration

- Added `ImGuiLayer` as a singleton overlay in `Application`.
- `PushOverlay(m_ImGuiLayer)` during `Application` construction.
- The render loop in `Application::Run()` calls `m_ImGuiLayer->Begin()`, iterates layers calling `OnImGuiRender()`, then calls `m_ImGuiLayer->End()`.

### ImGui Docking and Viewports

- Switched to the `docking` branch of ImGui.
- This allows ImGui windows to float outside the main application window and be docked together.
- Fixed a `LayerStack` bug: previously tracked an iterator pointer for layer insertion; changed to track an **index** instead for stability.

---

## Input Polling

**Goal:** Allow querying input state at any time (e.g., check if Ctrl + mouse are held simultaneously), as opposed to only reacting to events.

Polling = you ask for the current input state whenever you want:
```cpp
if (Input::IsKeyPressed(BF_KEY_SPACE)) { /* ... */ }
```

### Design

- `Input` is a **static singleton** — one global input manager, unlike `Window` which could have multiple instances.
- `Input.h` declares virtual functions (e.g., `IsKeyPressedImpl`, `IsMouseButtonPressedImpl`).
- `WindowsInput` extends `Input` and implements the functions using GLFW calls.
- The application exposes the window pointer via a static getter so `WindowsInput` can access it.

---

## Key and Mouse Codes

**Goal:** Remove direct GLFW key code references from Sandbox (and any game code).

Instead of:
```cpp
if (BerryFlux::Input::IsKeyPressed(GLFW_KEY_TAB)) // BAD — GLFW in Sandbox
```

Use:
```cpp
if (BerryFlux::Input::IsKeyPressed(BF_KEY_TAB))   // GOOD — engine abstraction
```

- Created `KeyCodes.h` and `MouseButtonCodes.h` with engine-specific `BF_KEY_*` and `BF_MOUSE_BUTTON_*` defines.
- These map to GLFW values internally but Sandbox never includes GLFW.

---

## Math Library: GLM

- Added **GLM** (OpenGL Mathematics) as the math library.
- Provides `vec2`, `vec3`, `vec4`, `mat4`, quaternions, and common math operations used for transforms, projections, and view matrices.

---

## Rendering

### Why GPU Rendering?

Rendering involves enormous numbers of pixels, vector and matrix math, and raycasting — operations that CPUs are slow at. GPUs have:
- Dedicated math processing units
- Massive parallelism (thousands of cores)
- Fast dedicated memory (VRAM)

We use a **Rendering API** (OpenGL, Vulkan, DirectX, Metal) to communicate with the GPU.

### The Multi-API Problem

No single rendering API works everywhere:

| API | Platform |
|---|---|
| OpenGL | Cross-platform (old standard) |
| Vulkan | Cross-platform (modern, complex) |
| DirectX 11/12 | Windows only |
| Metal | macOS / iOS only |

The engine abstracts the rendering API so game code never calls `glDrawElements` or `vkCmdDraw` directly.

### Rendering Architecture

```
┌─────────────────────────────────────────────────────────┐
│  Renderer (platform-independent)                        │
│  Scene graph, culling, materials, LOD, animation,       │
│  cameras, VFX, PostFX, reflections                      │
├─────────────────────────────────────────────────────────┤
│  Render API (per-platform implementation)               │
│  Context, swap chain, framebuffer, vertex buffer,       │
│  index buffer, texture, shader, pipeline, render pass   │
└─────────────────────────────────────────────────────────┘
```

The **Renderer** does not know which API is in use. It talks to the **Render API abstraction**, which dispatches to the correct implementation.

A **Render Command Queue** stores commands and executes them in batch — enabling future multi-threaded rendering where the render thread is one frame behind the update thread.

### Build Order

1. Start with OpenGL (simplest, cross-platform).
2. Build the Render API layer.
3. Build the Renderer on top.
4. Add more APIs (Vulkan, DirectX) later.

---

## Rendering Context

**Goal:** Wrap the concept of a graphics context so it's not tied to GLFW.

- GLFW manages the OpenGL context for us via `glfwMakeContextCurrent`.
- Created a `GraphicsContext` abstract class.
- `OpenGLContext` implements it and handles `Init()` (which calls GLAD's loader) and `SwapBuffers()`.
- `GLFWWindow` holds a `GraphicsContext*` and calls `SwapBuffers()` in `OnUpdate()`.
- This means when we add Vulkan, `GLFWWindow` just swaps in a `VulkanContext`.

---

## Rendering a Triangle

### OpenGL Shaders

Two shader types used for rendering:

- **Vertex Shader** — runs once per vertex. Positions the geometry.
- **Fragment Shader** — runs once per pixel (fragment). Fills the triangle with color.

Created `Shader.cpp`:
1. Write GLSL source strings for vertex and fragment shaders.
2. Compile them with `glCreateShader` / `glShaderSource` / `glCompileShader`.
3. Link them into a shader program with `glCreateProgram` / `glAttachShader` / `glLinkProgram`.
4. Bind with `glUseProgram(m_RendererID)`.

---

## Renderer API Abstraction

**Goal:** Remove raw OpenGL calls from `Application.cpp`. Make the renderer work with any API.

### Approach

- During **compile time**, detect which API is active and compile only that implementation.
- This avoids runtime branching (`if API == OpenGL`) on every frame, keeping the render loop fast.

### Shader Abstraction

- `Shader.h` — abstract base class.
- `OpenGLShader.cpp` — OpenGL-specific subclass, only compiled when OpenGL is the target API.
- `DirectXShader.cpp` would only compile on Windows.

### Buffer Abstraction

- `Buffer.h` — abstract `VertexBuffer` and `IndexBuffer` classes.
- `Buffer.cpp` — factory functions that check `Renderer::GetAPI()` and return the correct implementation.
- `OpenGLBuffer.cpp` — OpenGL vertex and index buffer implementations with `Bind()`, `Unbind()`, and data upload.
- Removed raw OpenGL buffer code from `Application.cpp` and replaced with the abstracted API.

---

## Vertex Buffer Layouts

**Goal:** Describe the layout of vertex data in a way any rendering API can understand.

### Components

- `ShaderDataType` enum — `Float`, `Float2`, `Float3`, `Float4`, `Mat3`, `Mat4`, `Int`, `Bool`, etc.
- `BufferElement` struct — stores type, name, size, offset, stride, normalization flag.
- `BufferLayout` class — a list of `BufferElement` entries. Calculates offsets and stride automatically.

### Usage

```cpp
BufferLayout layout = {
    { ShaderDataType::Float3, "a_Position" },
    { ShaderDataType::Float4, "a_Color" }
};
vertexBuffer->SetLayout(layout);
```

When binding the vertex array, iterate through the layout and call `glEnableVertexAttribArray` and `glVertexAttribPointer` for each element so the GPU knows how to read the interleaved vertex data.

Updated the shader program to accept per-vertex color, giving each vertex its own color.

---

## Vertex Arrays

A **Vertex Array Object (VAO)** stores:
- Pointers to vertex buffer(s)
- Pointer to index buffer
- The layout of each vertex buffer

This lets you switch between meshes by simply binding a different VAO instead of re-uploading data.

- Created abstract `VertexArray` class and `OpenGLVertexArray` implementation.
- Note: Vertex Arrays don't exist as a first-class concept in DirectX — we'll handle that later; for now the focus is OpenGL.

---

## Render Flow and Submission

**Goal:** Structure the rendering pipeline properly so the engine can optimize draw calls.

### Draw Call Flow

```
Renderer::BeginScene(camera, lights, environment)
  ↓
Renderer::Submit(shader, vertexArray, transform)  ← per object
  [adds to render command queue]
  ↓
Renderer::EndScene()
  ↓
[flush queue — execute draw calls in optimized order]
  ↓
glDrawElements(...)
```

### Files Created / Updated

- `Renderer.h` — `BeginScene`, `Submit`, `EndScene`.
- `RendererAPI.h` — abstract class determining which API is active.
- `RenderCommand.h` — static helpers (`DrawIndexed`, `SetClearColor`, `Clear`) that forward to the active `RendererAPI`.
- `OpenGLRendererAPI.cpp` — implements `RendererAPI` with actual `glDrawElements`, `glClearColor`, etc.
- `Application.cpp` updated to use `RenderCommand` rather than touching GLFW or GLAD directly.

---

## Cameras

### How Cameras Work

A camera contributes two matrices to the render pipeline:

- **View Matrix** — the inverse of the camera's world transform. If the camera moves left, the world appears to move right.
- **Projection Matrix** — encodes FOV, aspect ratio, near/far clip planes.

Final vertex position = **Projection × View × Model × VertexPosition**

(Note: GLM uses this order; DirectX uses the reverse.)

- **Projection × View** = calculated per camera.
- **Model** = calculated per object (transform).
- **VertexPos** = stored per vertex in the mesh.

When `BeginScene(camera)` is called, the view-projection matrix is uploaded to the shader as a **render uniform**. Each object submitted via `Submit()` uploads its own **model uniform** (transform).

### Orthographic Camera

Created `OrthographicCamera`:
- Takes left, right, bottom, top clip planes.
- Computes and caches a `ViewProjectionMatrix`.
- Recalculates when position or rotation changes.

---

## Moving Code to Sandbox

Camera initialization, shader creation, and per-frame camera movement code were moved from `Application` (engine) into `ExampleLayer` (Sandbox). The engine should not contain game-specific logic.

### Camera Movement (Smooth vs. Event-Based)

- Initial approach: Dispatched `OnKeyPressed` events to move the camera. This was **janky** — events fire once per key repeat, not smoothly.
- Correct approach: **Poll input in `OnUpdate()`** using `Input::IsKeyPressed()` for smooth, frame-rate-aligned movement.

---

## Timestep and Delta Time

**Goal:** Make movement speed frame-rate independent.

- Created `Timestep` class that stores time in **seconds** (and provides milliseconds conversion).
- In `Application::Run()`: calculate delta time each frame.
  ```cpp
  float time = (float)glfwGetTime(); // platform-specific; abstracted later
  Timestep timestep = time - m_LastFrameTime;
  m_LastFrameTime = time;
  ```
- Pass `timestep` to `OnUpdate(Timestep ts)` in every layer.
- In Sandbox, multiply movement speed by `ts`:
  ```cpp
  m_CameraPosition.x += m_CameraTranslationSpeed * ts;
  ```
  Now the camera moves the same real-world distance per second regardless of FPS.

---

## Transforms

**Goal:** Place objects at specific positions, rotations, and scales in the world.

- Vertex positions are relative to each other (object space). The **transform matrix** places the object in world space.
- Created a `mat4 transform` parameter in `Renderer::Submit()`.
- In the vertex shader, the final position = `u_ViewProjection × u_Transform × a_Position`.
- Added a `vec3 squarePosition` in `ExampleLayer` and moved it with WASD keys independently from the camera (camera uses arrow keys).

### Tile Grid

Created a grid by placing square rendering inside a nested for loop:
- Scale factor reduces each square to 10% of its original size.
- X and Y positions multiplied by a gap multiplier to space them evenly.

---

## Material System

**Goal:** Control the appearance of objects through flexible, reusable data rather than hardcoded shaders.

### Uniforms

**Uniforms** are pieces of data passed from the CPU to a shader at draw time (colors, texture slots, material properties). A **Material** bundles a shader with a set of uniform values.

### Implementation

- Added `UploadUniformFloat4(name, value)` to `Shader`.
- Created a `glm::vec4 redColor` and `blueColor` and uploaded them per square to create a checkerboard color pattern on the tile grid.

### Future Plans

- Create a `Material` class holding a shader + uniform buffer.
- Support `MaterialInstance` for per-object overrides of a base material.
- Sort draw calls by material to minimize shader binds (render all red objects, then all blue objects, etc.).

---

## Shader Abstraction and Uniforms

**Goal:** Remove all OpenGL code from `Shader.cpp` and support more uniform types.

### Steps

- Created `OpenGLShader` that derives from the abstract `Shader`.
- In `Sandbox`, cast the `Shader` pointer to `OpenGLShader` to access OpenGL-specific uniform setters.
- Added `SetInt`, `SetFloat`, `SetFloat3`, `SetFloat4`, `SetMat4` to `OpenGLShader`.
- Fixed a bug where the shader was not being bound before uploading uniforms.
- Added a color picker via ImGui UI to dynamically set the grid square colors at runtime.

---

## Refs, Scopes, and Smart Pointers

**Goal:** Define ownership semantics cleanly and prepare for multi-threaded rendering.

### Problem

In a multi-threaded renderer, the render thread runs one frame behind. This means vertex arrays and shaders submitted to the render queue cannot be deleted until the render thread is done with them — even if the game layer is done with them. We cannot rely on stack destruction or raw `delete`.

### Solution

In `Core.h`, defined aliases:

```cpp
namespace BerryFlux {
    template<typename T>
    using Scope = std::unique_ptr<T>;   // single owner, stack lifetime

    template<typename T>
    using Ref = std::shared_ptr<T>;     // shared ownership, ref-counted
}
```

This way, `Ref<VertexArray>` and `Scope<Window>` communicate ownership intent clearly. We can later swap in a custom allocator by changing just these two lines.

Updated all `shared_ptr` and `unique_ptr` usages throughout the engine to use `Ref` and `Scope`.

---

## Textures

**Goal:** Load image files and map them onto geometry.

### How Textures Work

A texture is a **buffer of memory stored on the GPU**. The shader samples from it during the fragment stage. Textures store not just color but also:
- Normal maps (surface direction for lighting)
- Roughness/shininess maps
- Emission maps
- Any data expressible as pixels

### Implementation Steps

1. **Modified vertex buffer** to include texture coordinates (`u`, `v`) for each corner of the quad.
2. **Updated vertex buffer layout** to include a `Float2` for texture coordinates.
3. **Created a texture shader** that outputs a gradient using UVs (to validate the layout works).
4. **Created abstract `Texture` and `Texture2D` classes.**
5. **Created `OpenGLTexture2D`** which loads images using `stb_image` (a header-only PNG/JPEG loader — `stb_image.h` pasted into vendor).
6. Bound the texture to a texture slot and passed the slot index to the shader as a uniform (`sampler2D`).
7. Fixed various bugs to get the checkerboard PNG rendering correctly on the quad.

### Image Loading (stb_image)

Rather than relying on OS image formats at runtime, the plan is to use a custom engine format. For now, PNG files are loaded via `stb_image`. In a production engine, assets would be pre-converted to an optimized format at build time.

---

## Blending

Enabled alpha blending in the renderer initialization so the GPU knows which fragment is in front when transparency is involved:

```cpp
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
```

---

## Shader Asset Files

**Goal:** Load shaders from files rather than hardcoding them as C++ strings.

### Why

- Shaders are assets — large, independently editable, and potentially hot-reloadable.
- Storing them as static strings in `.cpp` files makes them hard to edit and impossible to reload without recompiling.

### Implementation

- Added `Shader::Create(filepath)` factory function.
- `OpenGLShader` reads the shader file from disk via `ReadFile(filepath)`.
- A `Preprocess(source)` function parses the file and splits it into vertex and fragment sections using a `#type vertex` / `#type fragment` directive. Returns an `unordered_map<GLenum, std::string>`.
- Updated `Compile()` to accept the preprocessed map.
- Both string-based and file-based shader creation remain supported.
- Updated Sandbox to load shaders from files.

### Shader File Format

```glsl
#type vertex
#version 330 core
// ... vertex shader code ...

#type fragment
#version 330 core
// ... fragment shader code ...
```

---

## Shader Library

**Goal:** Centralized storage and retrieval for all shaders. Game code shouldn't have to manually hold `Ref<Shader>` pointers everywhere.

### Implementation

- Created `ShaderLibrary` class in `Shader.h`.
- API: `Add(shader)`, `Load(filepath)`, `Get(name)`.
- Internally stores shaders in an `unordered_map<std::string, Ref<Shader>>` (faster lookup than an array for named retrieval).
- Currently used in Sandbox; will move to the `Renderer` when it gains storage.
- Now you call `m_ShaderLibrary.Load("assets/shaders/Texture.glsl")` once and `m_ShaderLibrary.Get("Texture")` whenever needed.

---

## 2D Renderer Design

### Motivation

Even a 3D engine needs a 2D renderer for:
- UI overlays
- Main menus
- HUD elements
- Sprites

The 3D `Renderer::Submit` workflow (with full vertex/index buffer setup per object) is too heavy for 2D.

### Target API

```cpp
Renderer2D::BeginScene(camera);
Renderer2D::DrawQuad(position, size, color);
Renderer2D::DrawQuad(position, size, texture);
Renderer2D::EndScene();
```

### Batch Rendering

Instead of one draw call per quad, all quads are packed into a **single dynamic vertex buffer** updated every frame:
- 10,000 `DrawQuad` calls → **one draw call**
- Enormous GPU performance improvement

GPU texture slots: modern GPUs have ~32 texture slots (~8 used internally). If you have 100 textures, you flush and submit ~5 batches. With a **texture atlas** (many small textures packed into one large texture), you can minimize flushes. Sprite sheets are a common form of texture atlas.

### Additional 2D Subsystems (Future)

- **Animation** — sprite sheets with keyframes; delta-difference encoding between frames.
- **UI layout system** — handles screen resizing, phone/tablet scaling.
- **Text rendering** — font atlas packing.
- **PostFX** — HDR, bloom, color grading, blur.
- **Particle system.**

---

## Camera Controller

**Goal:** Provide a reusable, ready-to-use camera controller in the engine so Sandbox doesn't have to reimplement camera movement from scratch.

### Design Rationale

- `OrthographicCamera` is **pure data** — position, rotation, and the computed matrices.
- `OrthographicCameraController` **wraps** the camera and adds input + event handling on top.
- The controller never touches the renderer. It only manipulates camera data.
- Games with custom camera behavior can ignore the controller and manipulate `OrthographicCamera` directly.

### Implementation

Created `OrthographicCameraController`:

- Constructor takes `aspectRatio` and a `rotation` flag.
- Internally creates an `OrthographicCamera` with bounds calculated from `aspectRatio × zoomLevel`.
- `OnUpdate(Timestep ts)` polls arrow keys and rotates/translates the camera. Speed scales with zoom level.
- `OnEvent(Event&)` handles:
  - `MouseScrolledEvent` → update `m_ZoomLevel`, call `SetProjection`.
  - `WindowResizedEvent` → update `m_AspectRatio`, call `SetProjection`.
- `SetProjection()` recalculates the camera's projection bounds.
- `GetCamera()` returns a `const OrthographicCamera&` for passing to `Renderer::BeginScene`.

Updated Sandbox to use `OrthographicCameraController` instead of managing camera state manually.

---

## Window Resizing

**Goal:** Notify all engine systems when the window is resized or minimized.

### Steps

1. `Application` handles `WindowResizeEvent` — if width or height is 0 (window minimized), sets `m_Minimized = true`.
2. In `Application::Run()`, skip layer updates when `m_Minimized` is true (no point rendering to a zero-size window).
3. Added `Renderer::OnWindowResize(width, height)` which calls `RenderCommand::SetViewport(0, 0, width, height)`.
4. `RenderCommand::SetViewport` calls `RendererAPI::SetViewport`.
5. `OpenGLRendererAPI::SetViewport` calls `glViewport(x, y, width, height)` — tells OpenGL the new pixel area to render into.

---

## Preparing for 2D Rendering

**Goal:** Set up a clean, isolated test environment for the 2D renderer.

### Steps

1. Created `Sandbox2D` layer.
2. In `Sandbox.cpp`, pushed `Sandbox2D` instead of (or alongside) `ExampleLayer`.
3. Moved all 2D-relevant private variables from `ExampleLayer` to `Sandbox2D`.
4. Moved `ExampleLayer::OnAttach()` content into `Sandbox2D::OnAttach()`.
5. Converted remaining raw pointers to `Ref<VertexArray>`.
6. Simplified the scene to just what's needed to test the 2D renderer.

---

## Starting the 2D Renderer

**Goal:** Create `Renderer2D` — a clean, static, zero-instance-storage 2D rendering API.

### Design

`Renderer2D` is a **fully static class** — no instances, no copies. It wraps GPU commands and maintains its own internal state in a static struct.

### Implementation Steps

1. Created `Renderer2D.h` and `Renderer2D.cpp`.
2. Added static functions: `Init()`, `Shutdown()`, `BeginScene(camera)`, `EndScene()`, `DrawQuad(position, size, color)`, `DrawQuad(position, size, texture)`.
3. Created an internal `Renderer2DStorage` struct holding the flat-color quad's vertex array and shader, held as a static pointer.
4. `Init()` sets up the vertex array, vertex buffer with a unit quad, index buffer, and loads the flat-color shader.
5. `Shutdown()` deletes the storage struct.
6. `BeginScene(camera)` binds the shader and uploads the `u_ViewProjection` uniform.
7. `DrawQuad()` uploads the `u_Transform` uniform, binds the vertex array, and calls `RenderCommand::DrawIndexed`.
8. `Renderer::Init()` now also calls `Renderer2D::Init()`.
9. Updated `Sandbox2D` to use `Renderer2D::DrawQuad()` calls instead of manual vertex array setup.
10. Exposed `Renderer2D` in `BerryFlux.h`.

### TODOs at This Stage

- Implement `Shader::SetMat4` and `Shader::SetFloat4` as proper virtual interface functions to avoid dynamic casting to `OpenGLShader` throughout the codebase.
- Profiling / memory graph: visualize memory allocations per system.

---

## 2D Renderer Transforms

*(In progress — next step in development)*

---

*End of Notes*
