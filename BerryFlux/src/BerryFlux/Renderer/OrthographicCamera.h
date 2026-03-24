#pragma once 
#include <glm/glm.hpp>

namespace BerryFlux {

  class OrthographicCamera 
  {
    public:
      OrthographicCamera(float left, float right, float bottom, float top); //orthographic matrix and -1 for near and 1 for far

      const glm::vec3& GetPosition() const {return m_Position;}      
      void SetPosition(const glm::vec3& position) {m_Position = position; RecalculateViewMatrix();}

      float GetRotation() const {return m_Rotation;}
      void SetRotation(float rotation) {m_Rotation = rotation; RecalculateViewMatrix();}

      const glm::mat4& GetProjectionMatrix() const {return m_ProjectionMatrix;}
      const glm::mat4& GetViewMatrix() const {return m_ViewMatrix;}
      const glm::mat4& GetViewProjectionMatrix() const {return m_ViewProjectionMatrix;}
      
    private:
      void RecalculateViewMatrix(); //A function we call whenever we set rotation or position

    private:
      glm::mat4 m_ProjectionMatrix;
      glm::mat4 m_ViewMatrix;
      glm::mat4 m_ViewProjectionMatrix; //We request this often and we dont wanan do tre multiplication each time

      //Once per frame we have to calculate where the camera and for that we have to recalculate the view matrix
      //It will be inverse of actual transforamtion matrix
      glm::vec3 m_Position;
      float m_Rotation = 0.0f; //z rotation as right now it is a 2d camera
  };

}