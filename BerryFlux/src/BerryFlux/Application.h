#pragma once 

#include "Core.h"
#include "Events/Event.h"
#include "Window.h"
#include "BerryFlux/Events/ApplicationEvent.h"
#include "BerryFlux/LayerStack.h"

#include "BerryFlux/ImGui/ImGuiLayer.h"

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

      //To get the singleton reference from any part of the codebase
      inline static Application& Get() { return *s_Instance;}

      inline Window& GetWindow() {return *m_Window;}
    private:
      bool OnWindowClose(WindowCloseEvent& e);
      std::unique_ptr<Window> m_Window;
      ImGuiLayer* m_ImGuiLayer; //Adding ImGuiLayer as an explicit layer so that we dont have to manually add it instead automatically added
      bool m_Running = true;

      LayerStack m_LayerStack;

      unsigned int m_VertexArray, m_VertexBuffer, m_IndexBuffer;
      unsigned int m_ShaderProgram;
    private:
      static Application* s_Instance; //hold a singleton to our application as we can have only one application
  };

  //To be defined in Client
  Application* CreateApplication(); //Factory Pattern
}