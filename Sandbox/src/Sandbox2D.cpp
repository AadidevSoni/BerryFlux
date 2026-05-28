#include "Sandbox2D.h"
#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Platform/OpenGL/OpenGLShader.h"
#include <chrono>

template<typename Fn> //Fn is type of lamda
class Timer 
{
  public:
    Timer(const char* name, Fn&& func) : m_Name(name), m_Func(func), m_Stopped(false)
    {
      m_StartTimepoint = std::chrono::high_resolution_clock::now();
    }

    ~Timer()
    {
      if(!m_Stopped) {
        Stop();
      }
    }

    void Stop()
    {
      auto endTimepoint = std::chrono::high_resolution_clock::now();

      long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
      long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();
      
      m_Stopped = true;

      float duration = (end - start) * 0.001f;
      //Callback that lambda callback
      m_Func({m_Name, duration});
    }

  private:
    const char* m_Name;
    std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
    bool m_Stopped;
    Fn m_Func; 
};

//Making macro to make it easier to lamba callback
#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult profileResult) {m_ProfileResults.push_back(profileResult);})

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
  //Timer
  PROFILE_SCOPE("Sandbox2D::OnUpdate"); //static string

  //Update
  {
    PROFILE_SCOPE("CameraController::OnUpdate");
    m_CameraController.OnUpdate(ts);
  }

  //Render
  {
    PROFILE_SCOPE("Renderer Prep");
    BerryFlux::RenderCommand::SetClearColor({0.1f,0.1f,0.1f,1});
    BerryFlux::RenderCommand::Clear();
  }

  {
    PROFILE_SCOPE("Begin Scene");
    BerryFlux::Renderer2D::BeginScene(m_CameraController.GetCamera());
  }

  {
    PROFILE_SCOPE("Draw Quad 1");
    BerryFlux::Renderer2D::DrawQuad(
      {-0.5f, 0.0f, 0.0f},
      {0.8f, 0.8f},
      {0.8f,0.9f,0.3f,1.0f}
    );
  }

  {
    PROFILE_SCOPE("Draw Quad 2");
    BerryFlux::Renderer2D::DrawQuad(
      {0.5f, 1.0f, 0.0f},
      {0.3f, 0.8f},
      {0.2f,0.9f,0.8f,1.0f}
    );
  }
  /*
  for (int i = 0; i < 10; i++){
    for (int j = 0; j < 10; j++){
      float rotation = (i + j) * 0.2f;
      BerryFlux::Renderer2D::DrawQuad({ 0.0f + i, 0.0f + j },{ 0.8f, 0.8f },rotation,{ 0.8f + i*0.1, 0.2f + j*0.1, 0.3f + (i + j)*0.1, 1.0f});
    }
  }
    */

  BerryFlux::Renderer2D::DrawQuad({0.2f, 0.f,-0.1f}, {100.0f, 100.0f}, m_CheckerboardTexture);
  
    
  BerryFlux::Renderer2D::EndScene();
}
    
void Sandbox2D::OnImGuiRender()
{
  ImGui::Begin("Settings");
  ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor)); //This creates a color picker in the ImGui window for the square color
  
  //Go through al the profile results
  for(auto& result: m_ProfileResults) 
  {
    char label[50];
    strcpy(label, "%.3fms ");
    strcat(label, result.Name);
    ImGui::Text(label, result.Time);
  }
  m_ProfileResults.clear();
  
  ImGui::End();
}
    
void Sandbox2D::OnEvent(BerryFlux::Event& e)
{
  m_CameraController.OnEvent(e);
}