#include "Sandbox2D.h"
#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Platform/OpenGL/OpenGLShader.h"

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{

}

void Sandbox2D::OnAttach()
{
  m_SquareVA = BerryFlux::VertexArray::Create();

  float squareVertices [5 * 4] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f, //Bottom right corner has red color as red is 1 and green is 0
    0.5f, 0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f //If negative then 0 else positive and these are the 4 corners
  };

  BerryFlux::Ref<BerryFlux::VertexBuffer> squareVB;
  squareVB.reset(BerryFlux::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

  BerryFlux::BufferLayout squareVBLayout = {
    {BerryFlux::ShaderDataType::Float3, "aPos", true},
  };
  squareVB->SetLayout(squareVBLayout);
  m_SquareVA->AddVertexBuffer(squareVB);

  uint32_t squareIndices[6] = {0,1,2,2,3,0};
  BerryFlux::Ref<BerryFlux::IndexBuffer> squareIB;
  squareIB.reset(BerryFlux::IndexBuffer::Create(squareIndices, sizeof(squareIndices)/sizeof(uint32_t))); //passing as number as it is count of indices not size in bytes
  m_SquareVA->SetIndexBuffer(squareIB);

  m_FlatColorShader = BerryFlux::Shader::Create("assets/shaders/FlatColor.glsl");
}
    
void Sandbox2D::OnDetach()
{

}

void Sandbox2D::OnUpdate(BerryFlux::Timestep ts)
{
  //Update
  m_CameraController.OnUpdate(ts);

  //Render
  BerryFlux::RenderCommand::SetClearColor({0.1f,0.1f,0.1f,1});
  BerryFlux::RenderCommand::Clear();

  BerryFlux::Renderer::BeginScene(m_CameraController.GetCamera());
      
  std::dynamic_pointer_cast<BerryFlux::OpenGLShader>(m_FlatColorShader)->Bind(); //Bind the shader before setting the uniform
  std::dynamic_pointer_cast<BerryFlux::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);

  BerryFlux::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f))); //Render the square with the particular tranform

  BerryFlux::Renderer::EndScene();
}
    
void Sandbox2D::OnImGuiRender()
{
  ImGui::Begin("Settings");
  ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor)); //This creates a color picker in the ImGui window for the square color
  ImGui::End();
}
    
void Sandbox2D::OnEvent(BerryFlux::Event& e)
{
  m_CameraController.OnEvent(e);
}