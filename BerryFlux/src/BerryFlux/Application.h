#pragma once 

#include "Core.h"
#include "Events/Event.h"
#include "Window.h"
#include "BerryFlux/Events/ApplicationEvent.h"
#include "BerryFlux/LayerStack.h"

namespace BerryFlux {
  class BERRYFLUX_API Application //“Make this class visible outside the DLL” Without this, Sandbox couldn’t access engine classes.
  {
    //Every game inherits from this.
    public:
      Application();
      virtual ~Application(); //Required for inheritance safety.

      void Run();

      void OnEvent(Event& e);

      void PushLayer(Layer* layer);
      void PushOverlay(Layer* layer);
    private:
      bool OnWindowClose(WindowCloseEvent& e);
      std::unique_ptr<Window> m_Window;
      bool m_Running = true;

      LayerStack m_LayerStack;
  };

  //To be defined in Client
  Application* CreateApplication(); //Factory Pattern
}