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
  m_CheckerboardTexture = BerryFlux::Texture2D::Create("/Users/aadidev/Desktop/GameEngineDev/BerryFlux/Sandbox/assets/textures/checkse.png");
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
  //BerryFlux::Renderer2D::DrawQuad({0.0f, 0.0f, -1.0f}, {0.8f, 0.8f}, {0.8f,0.2f,0.3f,1.0f});
  //BerryFlux::Renderer2D::DrawQuad({0.0f, 0.0f, 1.0f}, {0.5f, 0.75f}, {0.2f,0.3f,0.8f,1.0f});
  
  for (int i = 0; i < 10; i++){
    for (int j = 0; j < 10; j++){
      float rotation = (i + j) * 0.2f;
      BerryFlux::Renderer2D::DrawQuad({ 0.0f + i, 0.0f + j },{ 0.8f, 0.8f },rotation,{ 0.8f + i*0.1, 0.2f + j*0.1, 0.3f + (i + j)*0.1, 1.0f});
    }
  }

  BerryFlux::Renderer2D::DrawQuad({0.2f, 0.f,-0.1f}, {100.0f, 100.0f}, m_CheckerboardTexture);
  
    
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