#pragma once 
#include "BerryFlux/Renderer/RendererAPI.h"

namespace BerryFlux {

  class OpenGLRendererAPI : public RendererAPI
  {
    public:
      virtual void SetClearColor(const glm::vec4& color) override; 
      virtual void Clear() override;

      virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;   
  };

}