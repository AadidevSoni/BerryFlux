#include "BerryFlux/Log.h"
#include "GLFWWindow.h"
#include "BerryFlux/Window.h"
#include "BerryFlux/Events/ApplicationEvent.h"
#include "BerryFlux/Events/MouseEvent.h"
#include "BerryFlux/Events/KeyEvent.h"

namespace BerryFlux {

  //This ensures GLFW is initialized only once.
  static bool s_GLFWInitialized = false;

  //It returns a pointer to a Window, but the object it actually creates is a GLFWWindow.
  Window* Window::Create(const WindowProps& props) {
    return new GLFWWindow(props);
  }

  GLFWWindow::GLFWWindow(const WindowProps& props) {
    Init(props);
  }

  GLFWWindow::~GLFWWindow(){
    Shutdown();
  }

  void GLFWWindow::Init(const WindowProps& props) {

    m_Data.Title = props.Title;
    m_Data.Width = props.Width;
    m_Data.Height = props.Height;

    BF_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

    //GLFW will only be initialized once but we can create mutliple windows
    if (!s_GLFWInitialized) {
      int success = glfwInit();
      //Assert if it fails
      BF_CORE_ASSERT(success, "could not initialize GLFW!");

      s_GLFWInitialized = true;
    }

    m_Window = glfwCreateWindow(
      (int)props.Width,
      (int)props.Height,
      m_Data.Title.c_str(),
      nullptr,
      nullptr
    );

    //This sets the window's OpenGL context as the current rendering context.
    glfwMakeContextCurrent(m_Window);
    //GLFW callbacks are C-style functions, meaning they cannot access class members directly.So you attach your data to the window.
    // Later inside callbacks you can retrieve it:
    glfwSetWindowUserPointer(m_Window, &m_Data); //pointer to struct of data to the callback function
    SetVSync(true);
  }

  void GLFWWindow::Shutdown() {
    glfwDestroyWindow(m_Window);
  }

  void GLFWWindow::OnUpdate() {
    glfwPollEvents();
    glfwSwapBuffers(m_Window); //Displays the rendered frame.
  }

  void GLFWWindow::SetVSync(bool enabled) {
    if(enabled) {
      //wait for monitor refresh before swapping buffers
      glfwSwapInterval(1); //wait 1 frame to be called
    }else {
      glfwSwapInterval(0);
    }

    m_Data.VSync = enabled;
  }

  bool GLFWWindow::IsVSync() const {
    return m_Data.VSync;
  }

}