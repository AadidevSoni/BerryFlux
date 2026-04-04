# BerryFlux

### ⚡ Real-Time C++ Game Engine

BerryFlux is a **modern C++ game engine** focused on **real-time rendering, modular architecture, and multi-API abstraction**.

> Built from scratch to explore how real engines like Unreal & Unity work under the hood.

---

## 🎯 Highlights

* 🧩 **Layered Engine Architecture**
* ⚡ **Custom Event System (bitmask-based)**
* 🖥️ **Platform Abstraction (GLFW decoupled)**
* 🎮 **Input System (polling + abstraction)**
* 🎨 **OpenGL Rendering Pipeline**
* 📦 **Renderer API Abstraction (future Vulkan/DX ready)**
* 🎥 **Camera + Transform System**
* 🧠 **Material & Shader System**
* 🖼️ **Texture Mapping (stb_image)**
* 🧰 **ImGui Integration (Docking + Viewports)**
* 🧵 **Custom Smart Pointer System (Ref / Scope)**

---

## 🏗️ Architecture

```text
Client (Sandbox)
        ↓
BerryFlux Engine
        ↓
Renderer (API-agnostic)
        ↓
Rendering API (OpenGL)
        ↓
Platform Layer (GLFW)
        ↓
Operating System
```

✔ Clean separation of concerns
✔ No platform/API leakage into core engine

---

## 🎨 Rendering Pipeline

```text
BeginScene(Camera)
   ↓
Submit(Object, Transform, Material)
   ↓
EndScene()
   ↓
Render Command Queue → GPU
```

* API-independent rendering flow
* Efficient submission system
* Designed for future multi-threaded rendering

---

## 🧠 Key Systems

### ⚡ Event System

* Type-safe dispatcher
* Bitmask-based categories
* Fully decoupled communication

### 🧱 Layer System

* Modular stacking (Game, UI, Debug)
* Event propagation (top → bottom)

### 🎮 Input System

* Polling-based input
* Custom key codes (no GLFW dependency in client)

### 🎥 Camera System

* View + Projection matrices
* Real-time movement with delta time

---

## 🛠️ Tech Stack

* **C++17**
* **OpenGL**
* **GLFW**
* **GLAD**
* **ImGui (Docking)**
* **GLM**
* **stb_image**
* **spdlog**
* **CMake**

---

## 📦 Project Structure

```text
BerryFlux
│
├── Core
├── Renderer
├── Platform
├── Sandbox
├── vendor
└── CMakeLists.txt
```

---

## ▶️ Build & Run

```bash
git clone <repo>
cd BerryFlux
mkdir build
cd build
cmake ..
cmake --build .
```

Run:

```bash
./bin/Sandbox
```

---

## 🚀 Roadmap

* Vulkan Renderer
* DirectX Support
* Entity Component System (ECS)
* Physics Engine
* Scene Graph
* Editor UI

---

## 👨‍💻 Author

**Aadidev Soni**

Game Engine Developer
Computer Science Student

---

> ⚡ BerryFlux is a step toward building a fully scalable, multi-platform game engine from scratch.
