#include "bfpch.h"
#include "Application.h"

#include "BerryFlux/Events/ApplicationEvent.h"
#include "BerryFlux/Log.h"

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

namespace BerryFlux {

  Application::Application() {
    m_Window = std::unique_ptr<Window>(Window::Create());
  }

  Application::~Application() {

  }

  void Application::Run() {
    while(m_Running) {
      glClearColor(1, 0, 1, 1);
      glClear(GL_COLOR_BUFFER_BIT);
      m_Window->OnUpdate();
    }
  }

}