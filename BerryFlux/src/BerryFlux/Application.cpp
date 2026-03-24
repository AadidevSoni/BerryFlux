#include "bfpch.h"
#include "Application.h"
#include "BerryFlux/Log.h"

#include "Input.h"

#include "BerryFlux/Renderer/Renderer.h"

namespace BerryFlux {

  //Call the function Application::x on this object and pass one argument when it is triggered.
  //This macro creates a callable function that binds a member function of Application
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

  Application* Application::s_Instance = nullptr;

  Application::Application() : m_Camera(-1.6f, 1.6f, -0.9f, 0.9f){
    BF_CORE_ASSERT(!s_Instance, "Application already exists!");
    s_Instance = this;

    m_Window = std::unique_ptr<Window>(Window::Create());
    m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent)); //This event callback goes into the WindowData and calls OnEvent function
  
    m_ImGuiLayer = new ImGuiLayer();
    //Adding to the layer stack
    PushOverlay(m_ImGuiLayer);

    //VertexArrays
    m_VertexArray.reset(VertexArray::Create());

    //Vertices we are not making a projection matrix so it goes by default screen positioning which is -1 to 1
    float vertices [3 * 7] = {
      -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, //3 pos for pos and 4 pos for color and this works as for vertex position, it checks using stride and offset
      0.5f, -0.5f, 0.0f, 1.0f, 0.2f, 0.3f, 0.4f,
      0.0f, 0.5f, 0.0f, 0.3f, 0.0f, 1.0f, 1.0f
    };

    std::shared_ptr<VertexBuffer> vertexBuffer;
    vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

    //To make layout not be directly accessible instead get it from the GetLayout
    {
      BufferLayout layout = {
        {ShaderDataType::Float3, "aPos", true},
        {ShaderDataType::Float4, "aColor", true}
      };
      vertexBuffer->SetLayout(layout);
    }

    m_VertexArray->AddVertexBuffer(vertexBuffer);

    uint32_t indices[3] = {0,1,2};
    std::shared_ptr<IndexBuffer> indexBuffer;
    indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint32_t))); //passing as number as it is count of indices not size in bytes
    m_VertexArray->SetIndexBuffer(indexBuffer);

    m_SquareVA.reset(VertexArray::Create());

    float squareVertices [3 * 4] = {
      -0.75f, -0.75f, 0.0f, 
      0.75f, -0.75f, 0.0f, 
      0.75f, 0.75f, 0.0f,
      -0.75f, 0.75f, 0.0f
    };

    std::shared_ptr<VertexBuffer> squareVB;
    squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

    BufferLayout squareVBLayout = {
        {ShaderDataType::Float3, "aPos", true}
      };
    squareVB->SetLayout(squareVBLayout);
    m_SquareVA->AddVertexBuffer(squareVB);

    uint32_t squareIndices[6] = {0,1,2,2,3,0};
    std::shared_ptr<IndexBuffer> squareIB;
    squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices)/sizeof(uint32_t))); //passing as number as it is count of indices not size in bytes
    m_SquareVA->SetIndexBuffer(squareIB);

    //Shader program
    std::string vertexSrc = R"(
    #version 410 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec4 aColor;

    uniform mat4 u_ViewProjection;

    out vec3 v_Position;
    out vec4 v_Color;

    void main()
    { 
      v_Position = aPos;
      v_Color = aColor;
      gl_Position = u_ViewProjection * vec4(aPos, 1.0);
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

    //Second shader for square which accepts no color per vertex
    std::string vertexSrc2 = R"(
    #version 410 core
    layout(location = 0) in vec3 aPos;

    uniform mat4 u_ViewProjection;

    out vec3 v_Position;

    void main()
    { 
      v_Position = aPos;
      gl_Position = u_ViewProjection * vec4(aPos, 1.0);
    }
    )";

    std::string fragmentSrc2 = R"(
    #version 410 core
    out vec4 FragColor;

    in vec3 v_Position;

    void main()
    {
      FragColor = vec4(0.7, 0.2, 0.2, 1.0);
    }
    )";

    m_Shader2.reset(new Shader(vertexSrc2, fragmentSrc2));
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
      RenderCommand::SetClearColor({0.1f,0.1f,0.1f,1});
      RenderCommand::Clear();

      m_Camera.SetPosition({0.5f, 0.5f, 0.0f}); //Moving cam right and up
      m_Camera.SetRotation(45.0f);

      Renderer::BeginScene(m_Camera);

      //Shader binding and vertex binding are done in the submissions now
      Renderer::Submit(m_Shader2, m_SquareVA);
      Renderer::Submit(m_Shader, m_VertexArray);

      Renderer::EndScene();

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