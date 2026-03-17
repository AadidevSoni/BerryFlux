#include "bfpch.h"
#include "Application.h"
#include "BerryFlux/Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Input.h"

namespace BerryFlux {

  //Call the function Application::x on this object and pass one argument when it is triggered.
  //This macro creates a callable function that binds a member function of Application
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

  Application* Application::s_Instance = nullptr;

  Application::Application() {
    BF_CORE_ASSERT(!s_Instance, "Application already exists!");
    s_Instance = this;

    m_Window = std::unique_ptr<Window>(Window::Create());
    m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent)); //This event callback goes into the WindowData and calls OnEvent function
  
    m_ImGuiLayer = new ImGuiLayer();
    //Adding to the layer stack
    PushOverlay(m_ImGuiLayer);

    //Vertex array
    glGenVertexArrays(1, &m_VertexArray);
    glBindVertexArray(m_VertexArray);

    //Vertices we are not making a projection matrix so it goes by default screen positioning which is -1 to 1
    float vertices [3 * 3] = {
      -0.5f, -0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
      0.0f, 0.5f, 0.0f
    };

    m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

    //So that GPU can read the data
    glEnableVertexAttribArray(0); //creating index 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    uint32_t indices[3] = {0,1,2};
    m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint32_t))); //passing as number as it is count of indices not size in bytes

    //Shader program
    std::string vertexSrc = R"(
    #version 410 core
    layout(location = 0) in vec3 aPos;
    out vec3 v_Position;

    void main()
    { 
      v_Position = aPos;
      gl_Position = vec4(aPos, 1.0);
    }
    )";

    std::string fragmentSrc = R"(
    #version 410 core
    out vec4 FragColor;
    in vec3 v_Position;

    void main()
    {
      FragColor = vec4(v_Position * 0.5 + 0.5, 1.0);
    }
    )";

    m_Shader.reset(new Shader(vertexSrc, fragmentSrc));
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
      int width, height;
      glfwGetFramebufferSize((GLFWwindow*)m_Window->GetNativeWindow(), &width, &height);
      glViewport(0, 0, width, height);

      glClearColor(0.1f,0.1f,0.1f,1);
      glClear(GL_COLOR_BUFFER_BIT);

      m_Shader->Bind();

      glBindVertexArray(m_VertexArray);
      glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

      for(Layer* layer: m_LayerStack) {
        layer->OnUpdate();
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