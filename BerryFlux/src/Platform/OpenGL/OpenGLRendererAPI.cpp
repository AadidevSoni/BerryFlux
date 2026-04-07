#include "bfpch.h"
#include "OpenGLRendererAPI.h" 

#include <glad/glad.h>

namespace BerryFlux {

  void OpenGLRendererAPI::Init()
  {
    glEnable(GL_BLEND); //Enable blending for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set the blending function for transparency

    glDisable(GL_DEPTH_TEST);
  }

  void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
  {
    glClearColor(color.r, color.g, color.b, color.a);
  }

  void OpenGLRendererAPI::Clear()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray)
  {
    glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
  }

}