<div align="center">

<img src="https://img.shields.io/badge/BerryFlux-Engine-ff6b6b?style=for-the-badge&labelColor=1a1a2e" alt="BerryFlux"/>

# ⚡ BerryFlux

### Open-Source · Collaborative · AI-Powered Game Engine

**Build games together. Ship faster. Own the platform.**

[![License](https://img.shields.io/badge/License-Apache_2.0-blue?style=flat-square)](LICENSE)
[![C++](https://img.shields.io/badge/C++-17-00599C?style=flat-square&logo=c%2B%2B)](https://isocpp.org/)
[![OpenGL](https://img.shields.io/badge/OpenGL-4.x-5586A4?style=flat-square&logo=opengl)](https://www.opengl.org/)
[![CMake](https://img.shields.io/badge/CMake-3.x-064F8C?style=flat-square&logo=cmake)](https://cmake.org/)
[![PRs Welcome](https://img.shields.io/badge/PRs-Welcome-brightgreen?style=flat-square)](CONTRIBUTING.md)
[![Stars](https://img.shields.io/github/stars/AadidevSoni/BerryFlux?style=flat-square&color=f4c430)](https://github.com/AadidevSoni/BerryFlux/stargazers)

[**🚀 Get Started**](#-build--run) · [**📖 Docs**](#-architecture) · [**🤝 Contribute**](#-contributing) · [**🗺️ Roadmap**](#-roadmap)

</div>

---

## What is BerryFlux?

BerryFlux is a **real-time C++ game engine** built from scratch — and built in the open.

Think of it as the **GitHub for game development**: a platform where developers can publish their game projects, browse and fork others' work, submit pull requests, and build together. BerryFlux ships with an **AI-powered prompt-to-code system** that lowers the barrier to entry for new game developers while giving veterans a serious toolkit to work with.

Under the hood, BerryFlux is a production-quality engine with a clean renderer abstraction, layered architecture, material system, and a 2D batch renderer — with Vulkan and ECS on the way.

> Built from scratch to understand — and eventually surpass — how Unreal & Unity work under the hood.

---

## ✨ Platform Vision

| Feature | Description |
|---|---|
| 🌐 **Open-Source Projects** | Publish your game project publicly. Let others discover, star, fork, and build on it |
| 🔀 **Pull Request Workflow** | Contributors submit engine/game changes. You review and merge what you like |
| 🤖 **AI Prompt-to-Code** | Describe what you want in plain English — BerryFlux generates the game code for you |
| 🤝 **Community Contributions** | Engine improvements, new renderers, physics backends — all through PRs |
| 📦 **Asset Pipelines** | Convert 3D models, textures, and shaders into engine-native formats at build time |
| 🔌 **Plugin System** *(coming soon)* | Ship reusable game systems as engine plugins others can drop into their projects |

---

## 🎯 Engine Highlights

- 🧩 **Layered Architecture** — Game, UI, Debug layers with clean event propagation
- ⚡ **Custom Event System** — Type-safe dispatcher with bitmask-based categories
- 🖥️ **Platform Abstraction** — GLFW decoupled; swap the window backend without touching engine code
- 🎮 **Polling Input System** — Custom key codes; zero GLFW dependency in client code
- 🎨 **OpenGL Rendering Pipeline** — Full render API abstraction, ready for Vulkan/DX12
- 🎥 **Camera + Transform System** — Orthographic camera controller with delta-time movement
- 🧠 **Material & Shader System** — Shader asset files, shader library, uniform API
- 🖼️ **Texture System** — stb_image, white-texture blending, tinting, UV scaling
- 🔲 **2D Batch Renderer** — Single-shader quads, texture + color in one draw call
- 🧰 **ImGui Integration** — Docking & floating viewports for debug/editor UI
- 🧵 **Smart Pointer Wrappers** — `Ref<T>` / `Scope<T>` for clean ownership semantics
- 🪵 **spdlog Logging** — Color-tagged engine and client consoles

---

## 🏗️ Architecture

```
Client (Sandbox / Your Game)
           ↓
  BerryFlux Engine Core
  ├── Event System
  ├── Layer Stack
  ├── Input Manager
  └── Application Loop
           ↓
     Renderer (API-agnostic)
     ├── Renderer2D (Batch)
     ├── Scene Submission
     └── Render Command Queue
           ↓
   Rendering API Abstraction
   ├── OpenGL (current)
   ├── Vulkan (planned)
   └── DirectX 12 (planned)
           ↓
    Platform Layer (GLFW)
           ↓
      Operating System
```

✔ No OpenGL in core engine code — only in `OpenGL*` implementation files  
✔ No GLFW in client code — only in `GLFWWindow`  
✔ Clean separation of platform, renderer, and application concerns

---

## 🎨 Rendering Pipeline

```
Renderer2D::BeginScene(camera)
        ↓
Renderer2D::DrawQuad(position, size, color/texture)
        ↓
       [batched into dynamic vertex buffer]
        ↓
Renderer2D::EndScene()
        ↓
  Single draw call → GPU
```

- **Batch rendering** — thousands of quads in one draw call
- **Single-shader design** — flat color and textures share one shader; switching minimized
- **Texture + color blending** — white texture pass-through, tinting support
- **Designed for future multi-threading** — render command queue architecture in place

---

## 🧠 Key Systems

### ⚡ Event System

Events are typed, categorized with bitmasks, and dispatched through a clean type-safe dispatcher. Application is the central hub; layers consume or pass events downward.

```cpp
EventCategoryInput       = BIT(0)  // 00001
EventCategoryMouse       = BIT(1)  // 00010
EventCategoryKeyboard    = BIT(2)  // 00100
// Combine freely: EventCategoryInput | EventCategoryMouse
```

### 🧱 Layer Stack

Layers are modular systems stacked in priority order. Events travel top → bottom and stop when consumed.

```
ImGuiLayer    ← receives events first (debug/editor)
GameLayer     ← gameplay logic
EngineLayer   ← lowest priority
```

### 🎮 Input System

Polling-based with a platform-independent API. No GLFW headers leak into your game code.

```cpp
if (Input::IsKeyPressed(BF_KEY_SPACE)) { /* jump */ }
if (Input::IsMouseButtonPressed(BF_MOUSE_BUTTON_LEFT)) { /* shoot */ }
```

### 🔲 2D Renderer

```cpp
Renderer2D::BeginScene(camera);
Renderer2D::DrawQuad({0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.2f, 0.2f, 1.0f});
Renderer2D::DrawQuad({1.5f, 0.0f}, {1.0f, 1.0f}, m_Texture);
Renderer2D::EndScene();
```

### 🧠 Shader & Material System

Shaders are loaded from `.glsl` asset files, stored in a `ShaderLibrary`, and addressed by name. Uniforms are set through a high-level API — no raw OpenGL in your game code.

```cpp
auto shader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");
shader->SetFloat4("u_Color", glm::vec4(1.0f, 0.5f, 0.2f, 1.0f));
shader->SetMat4("u_Transform", transform);
```

---

## 🛠️ Tech Stack

| Technology | Role |
|---|---|
| **C++17** | Core engine language |
| **OpenGL 4.x** | Current rendering backend |
| **GLFW** | Window & OS abstraction |
| **GLAD** | OpenGL function loader |
| **ImGui (Docking)** | Editor & debug UI |
| **GLM** | Math (vectors, matrices, transforms) |
| **stb_image** | Image loading (PNG, JPG, etc.) |
| **spdlog** | Structured engine logging |
| **CMake** | Cross-platform build system |

---

## 📦 Project Structure

```
BerryFlux/
│
├── BerryFlux/                  ← Engine source
│   ├── src/
│   │   ├── BerryFlux/
│   │   │   ├── Core/           ← Application, Layer, Events, Input, Log
│   │   │   ├── Renderer/       ← Renderer, Renderer2D, Shader, Texture, Buffer
│   │   │   └── Platform/       ← GLFWWindow, OpenGL implementations
│   │   └── BerryFlux.h         ← Public engine header (include this in your game)
│   └── vendor/                 ← GLFW, GLAD, ImGui, GLM, spdlog, stb
│
├── Sandbox/                    ← Example game / testbed
│   └── src/
│       ├── SandboxApp.cpp
│       ├── ExampleLayer.cpp    ← 3D demo
│       └── Sandbox2D.cpp       ← 2D renderer demo
│
├── CMakeLists.txt
└── README.md
```

---

## ▶️ Build & Run

**Prerequisites:** CMake 3.x, a C++17 compiler (GCC / Clang / MSVC), Git

```bash
# Clone with submodules
git clone https://github.com/AadidevSoni/BerryFlux.git
cd BerryFlux
git submodule update --init --recursive

# Build
mkdir build && cd build
cmake ..
cmake --build .

# Run the Sandbox
./bin/Sandbox
```

> **macOS note:** The engine builds as a `.dylib` shared library. Make sure your compiler supports C++17 (`-std=c++17`).

---

## 🤝 Contributing

BerryFlux is built in the open and contributions are the point.

Whether you're fixing a bug, implementing a new feature, improving documentation, or adding a new rendering backend — **your pull request is welcome here**.

### How to contribute

1. **Fork** the repository
2. **Create a branch** — `git checkout -b feature/your-feature-name`
3. **Make your changes** and commit with a clear message
4. **Push** to your fork and open a **Pull Request**
5. The maintainer reviews and merges what fits the engine's direction

### What we're looking for

- 🐛 Bug fixes and stability improvements
- ✨ New engine features (see roadmap below)
- 📖 Documentation and examples
- 🎮 Sandbox demos showing off engine capabilities
- 🔌 New rendering backends (Vulkan, DirectX 12, Metal)
- 🧪 Tests and benchmarks

> Check [Issues](https://github.com/AadidevSoni/BerryFlux/issues) for open tasks tagged `good first issue` or `help wanted`.

---

## 🗺️ Roadmap

### Engine Core
- [ ] Entity Component System (ECS)
- [ ] Scene Graph & Scene Serialization
- [ ] Physics Engine integration
- [ ] Audio System
- [ ] Virtual File System (VFS)

### Rendering
- [ ] Vulkan Backend
- [ ] DirectX 12 Backend
- [ ] 3D PBR Renderer
- [ ] Post-processing pipeline (Bloom, SSAO, Color Grading)
- [ ] Shadow Maps
- [ ] Particle System

### Platform & Editor
- [ ] Native Engine Editor (built with BerryFlux itself)
- [ ] Project system & asset browser
- [ ] Hot-reload for shaders and scripts
- [ ] Scripting language (Lua or visual scripting)

### AI Features
- [ ] **AI Prompt-to-Code** — describe gameplay in English, get working C++ / script output
- [ ] **AI Scene Generation** — generate level layouts from natural language
- [ ] **AI Asset Suggestions** — recommend shaders, materials, and assets based on context

### Platform (Open-Source Hub)
- [ ] Public project hosting & discovery
- [ ] Fork & pull request workflow for game projects
- [ ] Community asset marketplace
- [ ] Plugin registry

---

## 📜 License

BerryFlux is licensed under the **Apache License 2.0**. See [LICENSE](LICENSE) for full details.

You are free to use, modify, and distribute BerryFlux — including for commercial projects. Attribution appreciated.

---

<div align="center">

**Built by [Aadidev Soni](https://github.com/AadidevSoni)**  
Game Engine Developer · Computer Science Student

*BerryFlux is a step toward a fully open, collaborative, AI-assisted game development platform — built from scratch, one system at a time.*

⭐ **Star the repo** if you find it useful — it helps more developers discover BerryFlux.

</div>
