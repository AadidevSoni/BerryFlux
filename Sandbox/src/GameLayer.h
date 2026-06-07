#pragma once
#include "BerryFlux.h"
#include "Level.h"
#include <imgui.h>

class GameLayer : public BerryFlux::Layer {
  public:
    GameLayer();
    virtual ~GameLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(BerryFlux::Timestep ts) override;
    virtual void OnImGuiRender() override;
    void OnEvent(BerryFlux::Event& e) override;
    bool OnMouseButtonPressed(BerryFlux::MouseButtonPressedEvent& e);
    bool OnWindowResize(BerryFlux::WindowResizeEvent& e);

  private:
    void CreateCamera(uint32_t width, uint32_t height);

  private:
    BerryFlux::Scope<BerryFlux::OrthographicCamera> m_Camera;
    Level m_Level; //Contains: Player, Pillars, Collision, GameOver, Logic
    ImFont* m_Font;

    //Blink variables - Used to create flashing menu text.
    float m_Time = 0.0f;
    bool m_Blink = false;

    //MainMenu -> click -> Play -> death -> GameOver -> click -> Play

    enum class GameState
    {
      Play = 0, MainMenu = 1, GameOver = 2
    };

    GameState m_State = GameState::MainMenu;
};