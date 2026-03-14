#include "bfpch.h"
#include "OpenGLContext.h"

#include <glad/glad.h> //glad is openGL specific but GLFW isnt
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
  }

  void OpenGLContext::SwapBuffers() 
  {
    glfwSwapBuffers(m_WindowHandle); //Displays the rendered frame.
  }

}