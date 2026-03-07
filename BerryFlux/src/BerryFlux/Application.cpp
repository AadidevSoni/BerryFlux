#include "bfpch.h"
#include "Application.h"
#include "BerryFlux/Log.h"

#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

namespace BerryFlux {

  //Call the function Application::x on this object and pass one argument when it is triggered.
  //This macro creates a callable function that binds a member function of Application
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

  Application::Application() {
    m_Window = std::unique_ptr<Window>(Window::Create());
    m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent)); //This event callback goes into the WindowData and calls OnEvent function
  }

  Application::~Application() {

  }

  void Application::PushLayer(Layer* layer) {
    m_LayerStack.PushLayer(layer);
  }

  void Application::PushOverlay(Layer* layer) {
    m_LayerStack.PushOverlay(layer);
  }

  void Application::OnEvent(Event& e) {
    EventDispatcher dispatcher(e);
    //If the event is a WindowCloseEvent, call OnWindowClose()
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose)); //If the Event e is a window close event
    //Prints every event being logged
    BF_CORE_TRACE("{0}",e.ToString()); //prints what kind of event it is and data set to

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
      glClearColor(1, 0, 1, 1);
      glClear(GL_COLOR_BUFFER_BIT);

      for(Layer* layer: m_LayerStack) {
        layer->OnUpdate();
      }

      m_Window->OnUpdate();
    }
  }

  bool Application::OnWindowClose(WindowCloseEvent& e) {
    m_Running = false;
    return true;
  }

}