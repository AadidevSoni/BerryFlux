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

  static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
  {
    switch(type) 
    {
      case BerryFlux::ShaderDataType::Float:     return GL_FLOAT;
      case BerryFlux::ShaderDataType::Float2:    return GL_FLOAT;
      case BerryFlux::ShaderDataType::Float3:    return GL_FLOAT;
      case BerryFlux::ShaderDataType::Float4:    return GL_FLOAT;
      case BerryFlux::ShaderDataType::Mat3:      return GL_FLOAT;
      case BerryFlux::ShaderDataType::Mat4:      return GL_FLOAT;
      case BerryFlux::ShaderDataType::Int:       return GL_INT;
      case BerryFlux::ShaderDataType::Int2:      return GL_INT;
      case BerryFlux::ShaderDataType::Int3:      return GL_INT;
      case BerryFlux::ShaderDataType::Int4:      return GL_INT;
      case BerryFlux::ShaderDataType::Bool:      return GL_BOOL;
      case ShaderDataType::None: break;
    }
    BF_CORE_ASSERT(false, "Unknow shader data type!");
    return 0;
  }

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
    float vertices [3 * 7] = {
      -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, //3 pos for pos and 4 pos for color and this works as for vertex position, it checks using stride and offset
      0.5f, -0.5f, 0.0f, 1.0f, 0.2f, 0.3f, 0.4f,
      0.0f, 0.5f, 0.0f, 0.3f, 0.0f, 1.0f, 1.0f
    };

    m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

    //To make layout not be directly accessible instead get it from the GetLayout
    {
      BufferLayout layout = {
        {ShaderDataType::Float3, "aPos", true},
        {ShaderDataType::Float4, "aColor", true}
      };
      m_VertexBuffer->SetLayout(layout);
    }

    uint32_t index = 0;
    const auto& layout = m_VertexBuffer->GetLayout();
    for(const auto& element:layout)
    {
      //So that GPU can read the data
      glEnableVertexAttribArray(index); //creating index 
      glVertexAttribPointer(index, 
        element.GetComponentCount(), 
        ShaderDataTypeToOpenGLBaseType(element.Type), 
        element.Normalized ? GL_TRUE : GL_FALSE, 
        layout.GetStride(), 
        (const void*)element.Offset
      ); //Vertex attributes are stored in vertex array
      index++;
    }

    uint32_t indices[3] = {0,1,2};
    m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint32_t))); //passing as number as it is count of indices not size in bytes

    //Shader program
    std::string vertexSrc = R"(
    #version 410 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec4 aColor;

    out vec3 v_Position;
    out vec4 v_Color;

    void main()
    { 
      v_Position = aPos;
      v_Color = aColor;
      gl_Position = vec4(aPos, 1.0);
    }
    )";

    std::string fragmentSrc = R"(
    #version 410 core
    out vec4 FragColor;

    in vec3 v_Position;
    in vec4 v_Color;

    void main()
    {
      FragColor = vec4(v_Position * 0.5 + 0.5, 1.0);
      FragColor = v_Color;
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