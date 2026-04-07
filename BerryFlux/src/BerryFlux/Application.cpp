#include "bfpch.h"
#include "Application.h"
#include "BerryFlux/Log.h"

#include "Input.h"

#include "BerryFlux/Renderer/Renderer.h"

#include <GLFW/glfw3.h>

namespace BerryFlux {

  //Call the function Application::x on this object and pass one argument when it is triggered.
  //This macro creates a callable function that binds a member function of Application
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

  Application* Application::s_Instance = nullptr;

  Application::Application() 
  {
    BF_CORE_ASSERT(!s_Instance, "Application already exists!");
    s_Instance = this;

    m_Window = Scope<Window>(Window::Create());
    m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent)); //This event callback goes into the WindowData and calls OnEvent function
  
    Renderer::Init();

    m_ImGuiLayer = new ImGuiLayer();
    //Adding to the layer stack
    PushOverlay(m_ImGuiLayer);
  }

  Application::~Application() {

  }

  void Application::PushLayer(Layer* layer) {
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
  }

  void Application::PushOverlay(Layer* layer) {
    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
  }

  void Application::OnEvent(Event& e) {
    EventDispatcher dispatcher(e);
    //If the event is a WindowCloseEvent, call OnWindowClose()
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose)); //If the Event e is a window close event
    //Prints every event being logged
    //BF_CORE_TRACE("{0}",e.ToString()); //prints what kind of event it is and data set to

    //Backward iteration for event handling
    for(auto it = m_LayerStack.end();it != m_LayerStack.begin();) {
      (*--it)->OnEvent(e); //*it Dereference it and gives the layer name
      if(e.Handled) { //If an overlay ahndles an event then the layers wont get it
        break;
      }
    }
  }

  void Application::Run() {
    while(m_Running) {

      float time = (float)glfwGetTime();
      Timestep timestep = time - m_LastFrameTime;
      m_LastFrameTime = time;

      RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
      RenderCommand::Clear();

      for(Layer* layer: m_LayerStack) {
        layer->OnUpdate(timestep);
      }

      //Testing out Input Polling
      //auto[x,y] = Input::GetMousePosition();
      //BF_CORE_TRACE("{0}, {1}",x,y);

      m_ImGuiLayer->Begin();
      for(Layer* layer: m_LayerStack) {
        layer->OnImGuiRender();
      }
      m_ImGuiLayer->End();

      m_Window->OnUpdate();
    }
  }

  bool Application::OnWindowClose(WindowCloseEvent& e) {
    m_Running = false;
    return true;
  }

}