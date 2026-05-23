#pragma once

#include "BerryFlux.h"

class Sandbox2D : public BerryFlux::Layer 
{
  public:
    Sandbox2D();
    virtual ~Sandbox2D() = default;
    //This is where we do all the initilizations instead of done by the constructor
    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(BerryFlux::Timestep ts) override;
    virtual void OnImGuiRender() override;
    void OnEvent(BerryFlux::Event& e) override;

  private:
    BerryFlux::OrthographicCameraController m_CameraController;

    //Temp
    BerryFlux::Ref<BerryFlux::Shader> m_FlatColorShader;
    BerryFlux::Ref<BerryFlux::VertexArray> m_SquareVA;

    BerryFlux::Ref<BerryFlux::Texture2D> m_CheckerboardTexture; 

    glm::vec4 m_SquareColor = {0.2f, 0.3f, 0.8f, 1.0f}; //This is the color we will set in the shader uniform for the square
};