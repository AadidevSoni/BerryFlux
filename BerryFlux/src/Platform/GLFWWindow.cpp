#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "BerryFlux/Log.h"
#include "GLFWWindow.h"
#include "BerryFlux/Window.h"
#include "BerryFlux/Events/ApplicationEvent.h"
#include "BerryFlux/Events/MouseEvent.h"
#include "BerryFlux/Events/KeyEvent.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace BerryFlux {

  //This ensures GLFW is initialized only once.
  static bool s_GLFWInitialized = false;

  static void GLFWErrorCallback(int error, const char* description) {
    BF_CORE_ERROR("GLFW Error: ({0}): {1}", error, description);
  }

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
      glfwSetErrorCallback(GLFWErrorCallback);

      s_GLFWInitialized = true;
    }

    m_Window = glfwCreateWindow(
      (int)props.Width,
      (int)props.Height,
      m_Data.Title.c_str(),
      nullptr,
      nullptr
    );  

    //Creating and initializing the context
    m_Context = new OpenGLContext(m_Window);
    m_Context->Init(); 

    //GLFW callbacks are C-style functions, meaning they cannot access class members directly.So you attach your data to the window.
    // Later inside callbacks you can retrieve it:
    glfwSetWindowUserPointer(m_Window, &m_Data); //pointer to struct of data to the callback function
    SetVSync(true);

    //Set GLFW Callbacks Instead of writing a separate function: void Resize(GLFWwindow* window, int w, int h) You define it inline using a lambda:
    //We want this to call the application OnEvent function
    //GLFWwindow has a struct where we can put any data we want and we pointed to it DataStruct
    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) { //GLFW automatically passes these values.
      WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window); //get the user pointer from the window and type casting it
      data.Width = width;
      data.Height = height;

      WindowResizeEvent event(width, height); //engine creates a resize event object.
      data.EventCallback(event); //This calls the event callback function. So this actually becomes: Application::OnEvent(event)
    });

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
      WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
      WindowCloseEvent event;
      //dispatch
      data.EventCallback(event);
    });

    glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
      WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

      switch(action) {
        case GLFW_PRESS:
        { 
          KeyPressedEvent event(key, 0); //repeatcount = 0
          data.EventCallback(event);
          break;
        }
        case GLFW_RELEASE:
        { 
          KeyReleasedEvent event(key); //repeatcount = 0
          data.EventCallback(event);
          break;
        }
        case GLFW_REPEAT:
        { 
          KeyPressedEvent event(key, 1); //repeatcount = 0
          data.EventCallback(event);
          break;
        }
      }
    });
    
    glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
      WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

      KeyTypedEvent event(keycode);
      data.EventCallback(event);
    });

    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
      WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

      switch(action) {
        case GLFW_PRESS:
        {
          MouseButtonPressedEvent event(button);
          data.EventCallback(event);
          break;
        }
        case GLFW_RELEASE:
        {
          MouseButtonReleasedEvent event(button);
          data.EventCallback(event);
          break;
        }
      }
    });

    glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
      WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

      MouseScrolledEvent event((float)xOffset, (float)yOffset);
      data.EventCallback(event);
    });

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
      WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

      MouseMovedEvent event((float)xPos, (float)yPos);
      data.EventCallback(event);
    });
  }

  void GLFWWindow::Shutdown() {
    glfwDestroyWindow(m_Window);
  }

  void GLFWWindow::OnUpdate() {
    glfwPollEvents();
    m_Context->SwapBuffers();
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