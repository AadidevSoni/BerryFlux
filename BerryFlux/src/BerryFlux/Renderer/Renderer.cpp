#include "bfpch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace BerryFlux {

  Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

  void Renderer::BeginScene(OrthographicCamera& camera)
  {
    m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
  }

  void Renderer::EndScene()
  {
  }

  void Renderer::Submit(const BerryFlux::Ref<Shader>& shader, const BerryFlux::Ref<VertexArray>& vertexArray, const glm::mat4& transform)
  { 
    shader->Bind();
    std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
    std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform); //Submitted per objct

    vertexArray->Bind();
    RenderCommand::DrawIndexed(vertexArray);
  }

}