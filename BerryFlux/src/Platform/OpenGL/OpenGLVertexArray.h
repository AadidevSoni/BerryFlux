#pragma once 
#include "BerryFlux/Renderer/VertexArray.h"

namespace BerryFlux {

  class OpenGLVertexArray : public VertexArray{
    public:
      OpenGLVertexArray();
      virtual ~OpenGLVertexArray();

      virtual void Bind() const override;
      virtual void Unbind() const override;

      virtual void AddVertexBuffer(const BerryFlux::Ref<VertexBuffer>& vertexBuffer) override;
      virtual void SetIndexBuffer(const BerryFlux::Ref<IndexBuffer>& indexBuffer) override;

      virtual const std::vector<BerryFlux::Ref<VertexBuffer>>& GetVertexBuffers() const {return m_VertexBuffers;}
      virtual const BerryFlux::Ref<IndexBuffer>& GetIndexBuffer() const {return m_IndexBuffer;}
    private:
      uint32_t m_RendererID;
      std::vector<BerryFlux::Ref<VertexBuffer>> m_VertexBuffers; //Contains rederence to every vertex buffer we put in
      BerryFlux::Ref<IndexBuffer> m_IndexBuffer;
  };

}