#include "bfpch.h"
#include "OpenGLContext.h"

#include <glad/glad.h> //glad is openGL specific but GLFW isnt
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace BerryFlux {

  OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle) 
  {
    BF_CORE_ASSERT(windowHandle, "Window handle is null!");
  }

  void OpenGLContext::Init()
  {
    //This sets the window's OpenGL context as the current rendering context.
    glfwMakeContextCurrent(m_WindowHandle);

    //Initialize GLAD
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); //Use GLFW’s glfwGetProcAddress to fetch all OpenGL function pointers.
    BF_CORE_ASSERT(status, "Failed to initialize GLAD!");

    BF_CORE_INFO("OpenGL Info:");
    BF_CORE_INFO("  Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    BF_CORE_INFO("  Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    BF_CORE_INFO("  Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
  }

  void OpenGLContext::SwapBuffers() 
  {
    glfwSwapBuffers(m_WindowHandle); //Displays the rendered frame.
  }

}