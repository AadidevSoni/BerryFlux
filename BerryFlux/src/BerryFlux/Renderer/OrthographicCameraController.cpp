#include "bfpch.h"
#include "BerryFlux/Renderer/OrthographicCameraController.h"
#include "BerryFlux/Core/Input.h"
#include "BerryFlux/Core/KeyCodes.h"
#include "BerryFlux/Renderer/RenderCommand.h"

namespace BerryFlux {

  OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation) 
    : m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel , m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation) //left right bottom top
  {

  }

  void OrthographicCameraController::OnUpdate(Timestep ts)
  {
    if(Input::IsKeyPressed(BF_KEY_LEFT))
    {
      m_CameraPosition.x -= m_CameraTranslationSpeed * ts; 
    }else if(Input::IsKeyPressed(BF_KEY_RIGHT))
    {
      m_CameraPosition.x += m_CameraTranslationSpeed * ts;
    }

    if(Input::IsKeyPressed(BF_KEY_DOWN))
    {
      m_CameraPosition.y -= m_CameraTranslationSpeed * ts;
    }else if(Input::IsKeyPressed(BF_KEY_UP))
    {
      m_CameraPosition.y += m_CameraTranslationSpeed * ts;
    }
    
    if(m_Rotation) {
      if(Input::IsKeyPressed(BF_KEY_R))
      {
        m_CameraRotation -= m_CameraRotationSpeed * ts;
      }
      if(Input::IsKeyPressed(BF_KEY_E)){
        m_CameraRotation += m_CameraRotationSpeed * ts;
      }

      m_Camera.SetRotation(m_CameraRotation);
    }

    m_Camera.SetPosition(m_CameraPosition);

    m_CameraTranslationSpeed = m_ZoomLevel;
  }

  void OrthographicCameraController::OnEvent(Event& e) 
  {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(BF_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
    dispatcher.Dispatch<WindowResizeEvent>(BF_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
  }
  
  bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
  {
    m_ZoomLevel -= e.GetYOffset() * 0.5f;
    m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
    //Recompute everything just mofidy the zoomlevel value
    m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel , m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
    
    return false;
  }

  bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e) 
  {
    m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
    RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
    //modify the aspect ratio value
    m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel , m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
    
    return false;
  }

}