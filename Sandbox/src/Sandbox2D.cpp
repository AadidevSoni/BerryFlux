#include "Sandbox2D.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
    : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true) {}

void Sandbox2D::OnAttach() {
  BF_PROFILE_FUNCTION();

  m_CheckerboardTexture = BerryFlux::Texture2D::Create(
      "/Users/aadidev/Desktop/GameEngineDev/BerryFlux/Sandbox/assets/textures/"
      "checkse.png");
}

void Sandbox2D::OnDetach() { BF_PROFILE_FUNCTION(); }

void Sandbox2D::OnUpdate(BerryFlux::Timestep ts) {
  // Timer
  BF_PROFILE_FUNCTION();

  // Update
  m_CameraController.OnUpdate(ts);

  // Render
  {
    BF_PROFILE_SCOPE("Renderer Prep");
    BerryFlux::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    BerryFlux::RenderCommand::Clear();
  }

  {
    BF_PROFILE_SCOPE("Renderer Draw");
    BerryFlux::Renderer2D::BeginScene(m_CameraController.GetCamera());
    BerryFlux::Renderer2D::DrawQuad({-0.5f, 0.0f, 0.0f}, {0.8f, 0.8f},
                                    {1.0f, 0.0f, 0.0f, 1.0f});
    BerryFlux::Renderer2D::DrawQuad({0.5f, 1.0f, 0.0f}, {0.3f, 0.8f},
                                    {0.0f, 1.0f, 0.0f, 1.0f});
    BerryFlux::Renderer2D::DrawQuad({0.5f, -1.0f, 0.0f}, {0.8f, 0.2f},
                                    {0.0f, 0.0f, 1.0f, 1.0f});
    /*
    for (int i = 0; i < 10; i++){
        for (int j = 0; j < 10; j++){
          float rotation = (i + j) * 0.2f;
          BerryFlux::Renderer2D::DrawQuad({ 0.0f + i, 0.0f + j },{ 0.8f, 0.8f
    },rotation,{ 0.8f + i*0.1, 0.2f + j*0.1, 0.3f + (i + j)*0.1, 1.0f});
        }
      }
    */
    // Checker Pattern
    BerryFlux::Renderer2D::DrawQuad({0.0f, 0.0f, -0.1f}, {10.0f, 10.0f},
                                    m_CheckerboardTexture, 10.0f);

    // Rotation Animation
    static float rotation = 0.0f;
    rotation += ts * 20.0f;

    // Rotated Texture
    BerryFlux::Renderer2D::DrawQuad({1.5f, 0.0f, 0.1f}, {1.0f, 1.0f}, rotation,
                                    m_CheckerboardTexture, 20.0f);
    // Rotated quad
    BerryFlux::Renderer2D::DrawQuad({0.0f, 2.0f, 0.0f}, {1.0f, 1.0f}, 45.0f,
                                    {0.4f, 0.8f, 0.5f, 1.0f});
    BerryFlux::Renderer2D::EndScene();
  }
}

void Sandbox2D::OnImGuiRender() {
  BF_PROFILE_FUNCTION();

  ImGui::Begin("Settings");
  ImGui::ColorEdit4(
      "Square Color",
      glm::value_ptr(m_SquareColor)); // This creates a color picker in the
                                      // ImGui window for the square color

  ImGui::End();
}

void Sandbox2D::OnEvent(BerryFlux::Event &e) { m_CameraController.OnEvent(e); }