#include <BerryFlux.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"

class ExampleLayer : public BerryFlux::Layer {
  public:
    ExampleLayer():Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f), m_SquarePosition(0.0f)
    {
      //VertexArrays
      m_VertexArray.reset(BerryFlux::VertexArray::Create());

      //Vertices we are not making a projection matrix so it goes by default screen positioning which is -1 to 1
      float vertices [3 * 7] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, //3 pos for pos and 4 pos for color and this works as for vertex position, it checks using stride and offset
        0.5f, -0.5f, 0.0f, 1.0f, 0.2f, 0.3f, 0.4f,
        0.0f, 0.5f, 0.0f, 0.3f, 0.0f, 1.0f, 1.0f
      };

      BerryFlux::Ref<BerryFlux::VertexBuffer> vertexBuffer;
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
      BerryFlux::Ref<BerryFlux::IndexBuffer> indexBuffer;
      indexBuffer.reset(BerryFlux::IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint32_t))); //passing as number as it is count of indices not size in bytes
      m_VertexArray->SetIndexBuffer(indexBuffer);

      m_SquareVA.reset(BerryFlux::VertexArray::Create());

      float squareVertices [5 * 4] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, //Bottom right corner has red color as red is 1 and green is 0
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f //If negative then 0 else positive and these are the 4 corners
      };

      BerryFlux::Ref<BerryFlux::VertexBuffer> squareVB;
      squareVB.reset(BerryFlux::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

      BerryFlux::BufferLayout squareVBLayout = {
          {BerryFlux::ShaderDataType::Float3, "aPos", true},
          {BerryFlux::ShaderDataType::Float2, "aTexCoord", true}
        };
      squareVB->SetLayout(squareVBLayout);
      m_SquareVA->AddVertexBuffer(squareVB);

      uint32_t squareIndices[6] = {0,1,2,2,3,0};
      BerryFlux::Ref<BerryFlux::IndexBuffer> squareIB;
      squareIB.reset(BerryFlux::IndexBuffer::Create(squareIndices, sizeof(squareIndices)/sizeof(uint32_t))); //passing as number as it is count of indices not size in bytes
      m_SquareVA->SetIndexBuffer(squareIB);

      //Shader program
      std::string vertexSrc = R"(
      #version 410 core
      layout(location = 0) in vec3 aPos;
      layout(location = 1) in vec4 aColor;

      uniform mat4 u_ViewProjection;
      uniform mat4 u_Transform;

      out vec3 v_Position;
      out vec4 v_Color;

      void main()
      { 
        v_Position = aPos;
        v_Color = aColor;
        gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 1.0);
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

      m_Shader.reset(BerryFlux::Shader::Create(vertexSrc, fragmentSrc)); //we cannot do new shader as now it is an abstract class

      //Second shader for square which accepts no color per vertex
      std::string flatColorShaderVertexSrc = R"(
      #version 410 core
      layout(location = 0) in vec3 aPos;

      uniform mat4 u_ViewProjection;
      uniform mat4 u_Transform;

      out vec3 v_Position;

      void main()
      { 
        v_Position = aPos;
        gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 1.0);
      }
      )";

      std::string flatColorShaderFragmentSrc = R"(
      #version 410 core
      out vec4 FragColor;

      in vec3 v_Position;

      uniform vec4 u_Color; 

      void main()
      {
        FragColor = u_Color;
      }
      )";
      //Added a uniform for color in the fragment shader and we will set it from the application code

      m_FlatColorShader.reset(BerryFlux::Shader::Create(flatColorShaderVertexSrc, flatColorShaderFragmentSrc));

      //Texture Shader
      std::string textureShaderVertexSrc = R"(
      #version 410 core
      layout(location = 0) in vec3 aPos;
      layout(location = 1) in vec2 aTexCoord;

      uniform mat4 u_ViewProjection;
      uniform mat4 u_Transform;

      out vec2 v_TexCoord;

      void main()
      { 
          v_TexCoord = aTexCoord;
          gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 1.0);
      }
      )";

      std::string textureShaderFragmentSrc = R"(
      #version 410 core
      out vec4 FragColor;

      in vec2 v_TexCoord;

      uniform sampler2D u_Texture; //Uniform for the texture we will set from the application code

      void main()
      {
        //Outputing as color, first 2 components become the colro and x and y are red and green and 0 blue and 1 alpha
        //Visulaisation of data we put in vertex buffer as texture coordinates
        //FragColor = vec4(v_TexCoord, 0.0, 1.0);
        FragColor = texture(u_Texture, v_TexCoord); //Sample the texture with the texture coordinates and output it as the fragment color
      }
      )";
      //Added a uniform for color in the fragment shader and we will set it from the application code

      m_TextureShader.reset(BerryFlux::Shader::Create(textureShaderVertexSrc, textureShaderFragmentSrc));
      m_Texture = BerryFlux::Texture2D::Create("/Users/aadidev/Desktop/GameEngineDev/BerryFlux/Sandbox/assets/textures/Checkerboard.png"); //Creating a texture from the file path and storing it in the m_Texture variable
    
      std::dynamic_pointer_cast<BerryFlux::OpenGLShader>(m_TextureShader)->Bind(); //Bind the texture shader before setting the uniform
      std::dynamic_pointer_cast<BerryFlux::OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0); //Set the texture uniform to the texture slot 0
    }

    void OnUpdate(BerryFlux::Timestep ts) override 
    {
      //BF_TRACE("DeltaTime: {0}s ({1}ms)",ts.GetSeconds(), ts.GetMilliseconds());

      if(BerryFlux::Input::IsKeyPressed(BF_KEY_LEFT))
      {
        m_CameraPosition.x -= m_CameraSpeed * ts; 
      }else if(BerryFlux::Input::IsKeyPressed(BF_KEY_RIGHT))
      {
        m_CameraPosition.x += m_CameraSpeed * ts;
      }

      if(BerryFlux::Input::IsKeyPressed(BF_KEY_DOWN))
      {
        m_CameraPosition.y -= m_CameraSpeed * ts;
      }else if(BerryFlux::Input::IsKeyPressed(BF_KEY_UP))
      {
        m_CameraPosition.y += m_CameraSpeed * ts;
      }

      if(BerryFlux::Input::IsKeyPressed(BF_KEY_R))
      {
        m_CameraRotation -= m_CameraRotationSpeed * ts;
      }
      if(BerryFlux::Input::IsKeyPressed(BF_KEY_E)){
        m_CameraRotation += m_CameraRotationSpeed * ts;
      }

      if(BerryFlux::Input::IsKeyPressed(BF_KEY_A))
      {
        m_SquarePosition.x -= m_SquareSpeed * ts; 
      }else if(BerryFlux::Input::IsKeyPressed(BF_KEY_D))
      {
        m_SquarePosition.x += m_SquareSpeed * ts;
      }

      if(BerryFlux::Input::IsKeyPressed(BF_KEY_S))
      {
        m_SquarePosition.y -= m_SquareSpeed * ts;
      }else if(BerryFlux::Input::IsKeyPressed(BF_KEY_W))
      {
        m_SquarePosition.y += m_SquareSpeed * ts;
      }

      BerryFlux::RenderCommand::SetClearColor({0.1f,0.1f,0.1f,1});
      BerryFlux::RenderCommand::Clear();

      m_Camera.SetPosition(m_CameraPosition); //Moving cam right and up
      m_Camera.SetRotation(m_CameraRotation);

      BerryFlux::Renderer::BeginScene(m_Camera);

      //Triangle
      //BerryFlux::Renderer::Submit(m_Shader, m_VertexArray); //Render the triangle with the default transform

      //glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_SquarePosition); //Translation matrix for square
      static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)); //Scaling matrix for square to 10% of the current size

      //Shader binding and vertex binding are done in the submissions now
      glm::vec4 redColor(0.8f, 0.2f, 0.3f, 1.0f); 
      glm::vec4 blueColor(0.2f, 0.3f, 0.8f, 1.0f);
      
      std::dynamic_pointer_cast<BerryFlux::OpenGLShader>(m_FlatColorShader)->Bind(); //Bind the shader before setting the uniform

      int gridSize = 30;
      float gridSpacing = 0.11f; //Spacing between squares in the grid
      for(int i=0;i<gridSize;i++) 
      {
        for(int j=0;j<gridSize;j++) 
        {
          float x = (i - gridSize / 2) * gridSpacing;
          float y = (j - gridSize / 2) * gridSpacing;

          glm::vec3 pos(x, y, 0.0f); //Position for each square
          glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos + m_SquarePosition) * scale; //Translation and scaling for each square
          if((i+j) % 3 == 0) {
            std::dynamic_pointer_cast<BerryFlux::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", redColor);
          } else if((i+j) % 3 == 1) {
            std::dynamic_pointer_cast<BerryFlux::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", blueColor);
          }else {
            std::dynamic_pointer_cast<BerryFlux::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", glm::vec4(m_SquareColor, 1.0f)); //Set the color uniform to the square color variable
          }
          BerryFlux::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform); //Render the square with the particular tranform
        } 
      }

      //Big Texture Square
      std::dynamic_pointer_cast<BerryFlux::OpenGLShader>(m_TextureShader)->Bind(); // ✅ bind shader FIRST
      m_Texture->Bind(0); // then bind texture
      BerryFlux::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f))); //Render the square with the particular tranform

      BerryFlux::Renderer::EndScene();
    }

    void OnImGuiRender() override 
    {
      ImGui::Begin("Settings");
      ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor)); //This creates a color picker in the ImGui window for the square color
      ImGui::End();
    }

    void OnEvent(BerryFlux::Event& event) override 
    {
      
    }

    private:
      BerryFlux::Ref<BerryFlux::Shader> m_Shader;
      BerryFlux::Ref<BerryFlux::VertexArray> m_VertexArray;

      BerryFlux::Ref<BerryFlux::Shader> m_FlatColorShader, m_TextureShader;
      BerryFlux::Ref<BerryFlux::VertexArray> m_SquareVA;

      BerryFlux::Ref<BerryFlux::Texture2D> m_Texture;

      BerryFlux::OrthographicCamera m_Camera;
      glm::vec3 m_CameraPosition;
      float m_CameraSpeed = 1.0f;
      float m_CameraRotation = 0.0f;
      float m_CameraRotationSpeed = 5.0f;

      glm::vec3 m_SquarePosition; 
      float m_SquareSpeed = 0.5f;

      glm::vec3 m_SquareColor = {0.4f, 0.9f, 0.4f}; //This is the color we will set in the shader uniform for the square
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