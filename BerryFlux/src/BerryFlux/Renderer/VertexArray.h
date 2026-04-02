#pragma once 

#include <memory>
#include "BerryFlux/Renderer/Buffer.h"

namespace BerryFlux {

  class VertexArray {
    public:
      virtual ~VertexArray() {}

      virtual void Bind() const = 0;
      virtual void Unbind() const = 0;

      virtual void AddVertexBuffer(const BerryFlux::Ref<VertexBuffer>& vertexBuffer) = 0;
      virtual void SetIndexBuffer(const BerryFlux::Ref<IndexBuffer>& indexBuffer) = 0;

      virtual const std::vector<BerryFlux::Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
      virtual const BerryFlux::Ref<IndexBuffer>& GetIndexBuffer() const = 0;

      static VertexArray* Create();
  };

}