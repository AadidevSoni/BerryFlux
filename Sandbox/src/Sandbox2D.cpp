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

  BerryFlux::Renderer2D::BeginScene(m_CameraController.GetCamera());
  BerryFlux::Renderer2D::DrawQuad({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.8f,0.2f,0.3f,1.0f});
      
  //std::dynamic_pointer_cast<BerryFlux::OpenGLShader>(m_FlatColorShader)->Bind(); //Bind the shader before setting the uniform
  //std::dynamic_pointer_cast<BerryFlux::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);

  BerryFlux::Renderer2D::EndScene();
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