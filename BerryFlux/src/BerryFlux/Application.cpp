#include "bfpch.h"
#include "Application.h"

#include "BerryFlux/Events/ApplicationEvent.h"
#include "BerryFlux/Log.h"

namespace BerryFlux {

  Application::Application() {

  }

  Application::~Application() {

  }

  void Application::Run() {
    WindowResizeEvent e(1280, 720);
    BF_TRACE(e.ToString()); //logging the window resize event
    while(true);
  }

}