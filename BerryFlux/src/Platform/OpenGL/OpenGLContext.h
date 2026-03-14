#pragma once

#include "BerryFlux/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace BerryFlux {

  class OpenGLContext : public GraphicsContext {
    public:
      OpenGLContext(GLFWwindow* windowHandle); //We need a context to the actual window

      virtual void Init() override;
      virtual void SwapBuffers() override; 
    private:
      GLFWwindow* m_WindowHandle;
  }; 

}