#include "GameLayer.h"
#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace BerryFlux;

GameLayer::GameLayer() : Layer("GameLayer") {
  auto &window = Application::Get().GetWindow();
  CreateCamera(window.GetWidth(), window.GetHeight());
  Random::Init();
}

void GameLayer::OnAttach() {
  m_Level.Init();

  // Loads a large font
  ImGuiIO io = ImGui::GetIO();
  m_Font = io.Fonts->AddFontFromFileTTF("assets/OpenSans-Regular.ttf", 120.0f);
}

void GameLayer::OnDetach() {}

// Runs every frame
void GameLayer::OnUpdate(BerryFlux::Timestep ts) {
  m_Time += ts; // Accumulate elapsed time.

  // Creates flashing text.
  m_Blink = ((int)(m_Time * 2.0f) % 2) == 0;

  if (m_Level.IsGameOver()) {
    m_State = GameState::GameOver;
  }

  // Camera follow
  const auto &playerPos = m_Level.GetPlayer().GetPosition();

  float cameraY = glm::clamp(playerPos.y, -2.0f, 2.0f);

  m_Camera->SetPosition({playerPos.x, cameraY, 0.0f});

  switch (m_State) {
  case GameState::Play: // Only update gameplay during Play state
  {
    m_Level.OnUpdate(ts);
    break;
  }
  }

  // Render
  BerryFlux::RenderCommand::SetClearColor(
      {0.0f, 0.0f, 0.0f, 1});        // Black background
  BerryFlux::RenderCommand::Clear(); // Clear framebuffer

  // Everything rendered relative to camera.
  BerryFlux::Renderer2D::BeginScene(*m_Camera);
  m_Level.OnRender(); // Draws everything
  BerryFlux::Renderer2D::EndScene();
}

void GameLayer::OnImGuiRender() {
  // ImGui::Begin("Settings");
  // m_Level.OnImGuiRender();
  // ImGui::End();

  // UI?

  switch (m_State) {
  case GameState::Play: {
    uint32_t playerScore = m_Level.GetPlayer().GetScore();
    std::string scoreStr = std::string("Score: ") + std::to_string(playerScore);
    ImGui::GetForegroundDrawList()->AddText(
        m_Font, 48.0f, ImGui::GetWindowPos(), 0xffffffff, scoreStr.c_str());
    break;
  }

  case GameState::MainMenu: {
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    const char *text = "Click to Play!";

    ImVec2 textSize = m_Font->CalcTextSizeA(120.0f, FLT_MAX, 0.0f, text);

    ImVec2 pos;
    pos.x = (displaySize.x - textSize.x) * 0.5f;
    pos.y = displaySize.y * 0.25f;

    if (m_Blink) {
      ImGui::GetForegroundDrawList()->AddText(
          m_Font, 120.0f, pos, IM_COL32(255, 255, 255, 255), text);
    }

    break;
  }
  case GameState::GameOver: {
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    // Click to Play
    const char *title = "Click to Play!";

    ImVec2 titleSize = m_Font->CalcTextSizeA(120.0f, FLT_MAX, 0.0f, title);

    ImVec2 titlePos;
    titlePos.x = (displaySize.x - titleSize.x) * 0.5f;
    titlePos.y = displaySize.y * 0.25f;

    if (m_Blink) {
      ImGui::GetForegroundDrawList()->AddText(
          m_Font, 120.0f, titlePos, IM_COL32(255, 255, 255, 255), title);
    }

    // Score
    uint32_t playerScore = m_Level.GetPlayer().GetScore();
    std::string scoreStr = "Score: " + std::to_string(playerScore);

    ImVec2 scoreSize =
        m_Font->CalcTextSizeA(48.0f, FLT_MAX, 0.0f, scoreStr.c_str());

    ImVec2 scorePos;
    scorePos.x = (displaySize.x - scoreSize.x) * 0.5f;
    scorePos.y = displaySize.y * 0.45f;

    ImGui::GetForegroundDrawList()->AddText(m_Font, 48.0f, scorePos,
                                            IM_COL32(255, 255, 255, 255),
                                            scoreStr.c_str());

    break;
  }
  }
}

void GameLayer::OnEvent(BerryFlux::Event &e) {
  EventDispatcher dispatcher(e);
  dispatcher.Dispatch<WindowResizeEvent>(
      BF_BIND_EVENT_FN(GameLayer::OnWindowResize));
  dispatcher.Dispatch<MouseButtonPressedEvent>(
      BF_BIND_EVENT_FN(GameLayer::OnMouseButtonPressed));
}

bool GameLayer::OnMouseButtonPressed(BerryFlux::MouseButtonPressedEvent &e) {
  std::cout << "CLICK!" << std::endl;
  if (m_State == GameState::GameOver)
    m_Level.Reset();

  m_State = GameState::Play;
  return false;
}

bool GameLayer::OnWindowResize(BerryFlux::WindowResizeEvent &e) {
  CreateCamera(e.GetWidth(), e.GetHeight());
  return false;
}

void GameLayer::CreateCamera(uint32_t width, uint32_t height) {
  float aspectRatio = (float)width / (float)height;

  // Manually fixing the section of area visible in the camera
  float camWidth = 8.0f;
  float bottom = -camWidth;
  float top = camWidth;
  float left = bottom * aspectRatio;
  float right = top * aspectRatio;
  m_Camera = CreateScope<OrthographicCamera>(left, right, bottom, top);
}