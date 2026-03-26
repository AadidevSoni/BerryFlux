#include <BerryFlux.h>

class ExampleLayer : public BerryFlux::Layer {
  public:
    ExampleLayer():Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)
    {
      //VertexArrays
      m_VertexArray.reset(BerryFlux::VertexArray::Create());

      //Vertices we are not making a projection matrix so it goes by default screen positioning which is -1 to 1
      float vertices [3 * 7] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, //3 pos for pos and 4 pos for color and this works as for vertex position, it checks using stride and offset
        0.5f, -0.5f, 0.0f, 1.0f, 0.2f, 0.3f, 0.4f,
        0.0f, 0.5f, 0.0f, 0.3f, 0.0f, 1.0f, 1.0f
      };

      std::shared_ptr<BerryFlux::VertexBuffer> vertexBuffer;
      vertexBuffer.reset(BerryFlux::VertexBuffer::Create(vertices, sizeof(vertices)));

      //To make layout not be directly accessible instead get it from the GetLayout
      {
        BerryFlux::BufferLayout layout = {
          {BerryFlux::ShaderDataType::Float3, "aPos", true},
          {BerryFlux::ShaderDataType::Float4, "aColor", true}
        };
        vertexBuffer->SetLayout(layout);
      }

      m_VertexArray->AddVertexBuffer(vertexBuffer);

      uint32_t indices[3] = {0,1,2};
      std::shared_ptr<BerryFlux::IndexBuffer> indexBuffer;
      indexBuffer.reset(BerryFlux::IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint32_t))); //passing as number as it is count of indices not size in bytes
      m_VertexArray->SetIndexBuffer(indexBuffer);

      m_SquareVA.reset(BerryFlux::VertexArray::Create());

      float squareVertices [3 * 4] = {
        -0.75f, -0.75f, 0.0f, 
        0.75f, -0.75f, 0.0f, 
        0.75f, 0.75f, 0.0f,
        -0.75f, 0.75f, 0.0f
      };

      std::shared_ptr<BerryFlux::VertexBuffer> squareVB;
      squareVB.reset(BerryFlux::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

      BerryFlux::BufferLayout squareVBLayout = {
          {BerryFlux::ShaderDataType::Float3, "aPos", true}
        };
      squareVB->SetLayout(squareVBLayout);
      m_SquareVA->AddVertexBuffer(squareVB);

      uint32_t squareIndices[6] = {0,1,2,2,3,0};
      std::shared_ptr<BerryFlux::IndexBuffer> squareIB;
      squareIB.reset(BerryFlux::IndexBuffer::Create(squareIndices, sizeof(squareIndices)/sizeof(uint32_t))); //passing as number as it is count of indices not size in bytes
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

      m_Shader.reset(new BerryFlux::Shader(vertexSrc, fragmentSrc));

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

      m_Shader2.reset(new BerryFlux::Shader(vertexSrc2, fragmentSrc2));
    }

    void OnUpdate() override 
    {
      if(BerryFlux::Input::IsKeyPressed(BF_KEY_LEFT))
      {
        m_CameraPosition.x -= m_CameraSpeed;
      }else if(BerryFlux::Input::IsKeyPressed(BF_KEY_RIGHT))
      {
        m_CameraPosition.x += m_CameraSpeed;
      }

      if(BerryFlux::Input::IsKeyPressed(BF_KEY_DOWN))
      {
        m_CameraPosition.y -= m_CameraSpeed;
      }else if(BerryFlux::Input::IsKeyPressed(BF_KEY_UP))
      {
        m_CameraPosition.y += m_CameraSpeed;
      }

      if(BerryFlux::Input::IsKeyPressed(BF_KEY_R))
      {
        m_CameraRotation -= m_CameraRotationSpeed;
      }else if(BerryFlux::Input::IsKeyPressed(BF_KEY_E)){
        m_CameraRotation += m_CameraRotationSpeed;
      }

      BerryFlux::RenderCommand::SetClearColor({0.1f,0.1f,0.1f,1});
      BerryFlux::RenderCommand::Clear();

      m_Camera.SetPosition(m_CameraPosition); //Moving cam right and up
      m_Camera.SetRotation(m_CameraRotation);

      BerryFlux::Renderer::BeginScene(m_Camera);

      //Shader binding and vertex binding are done in the submissions now
      BerryFlux::Renderer::Submit(m_Shader2, m_SquareVA);
      BerryFlux::Renderer::Submit(m_Shader, m_VertexArray);

      BerryFlux::Renderer::EndScene();
    }

    void OnEvent(BerryFlux::Event& event) override 
    {
    }

    private:
      std::shared_ptr<BerryFlux::Shader> m_Shader;
      std::shared_ptr<BerryFlux::VertexArray> m_VertexArray;

      std::shared_ptr<BerryFlux::Shader> m_Shader2;
      std::shared_ptr<BerryFlux::VertexArray> m_SquareVA;

      BerryFlux::OrthographicCamera m_Camera;
      glm::vec3 m_CameraPosition;
      float m_CameraSpeed = 0.05f;
      float m_CameraRotation = 0.0f;
      float m_CameraRotationSpeed = 0.05f;
};

class Sandbox : public BerryFlux::Application {
  public:
    Sandbox() {
      PushLayer(new ExampleLayer());
    }

    ~Sandbox() {

    }
};
//This overrides the engine factory. So EntryPoint doesn’t know about Sandbox.
BerryFlux::Application* BerryFlux::CreateApplication() {
  return new Sandbox();
}