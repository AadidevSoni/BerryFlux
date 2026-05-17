#pragma once 
#include "BerryFlux/Renderer/OrthographicCamera.h"
#include "BerryFlux/Core/Timestep.h"
#include "BerryFlux/Events/ApplicationEvent.h" //For window resize
#include "BerryFlux/Events/MouseEvent.h" //For scrolling

namespace BerryFlux {

  class OrthographicCameraController {
    public:
      OrthographicCameraController(float aspectRatio, bool rotation = false); //zoom level - 1, aspect ratio * 2 units

      void OnUpdate(Timestep ts);
      void OnEvent(Event& e);

      OrthographicCamera& GetCamera() {return m_Camera;}
      const OrthographicCamera& GetCamera() const {return m_Camera;}
    private:
      bool OnMouseScrolled(MouseScrolledEvent& e);
      bool OnWindowResized(WindowResizeEvent& e);
    private:
      float m_AspectRatio;
      float m_ZoomLevel = 1.0f;
      OrthographicCamera m_Camera;

      bool m_Rotation;

      glm::vec3 m_CameraPosition = {0.0f, 0.0f, 0.0f};
      float m_CameraRotation = 0.0f;
      float m_CameraTranslationSpeed = 1.0f, m_CameraRotationSpeed = 5.0f;
  };

}