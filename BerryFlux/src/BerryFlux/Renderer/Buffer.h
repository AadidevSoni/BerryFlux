#pragma once

namespace BerryFlux { //This is just an interface anddoesnt store any data

  class VertexBuffer {
    public:
      virtual ~VertexBuffer() {}

      virtual void Bind() const = 0;
      virtual void Unbind() const = 0;

      //Static coz it does not belong to this calss but this way of contructor so that we can specify our type
      static VertexBuffer* Create(float* vertices, uint32_t size);
  };

  class IndexBuffer {
    public:
      virtual ~IndexBuffer() {}

      virtual void Bind() const = 0;
      virtual void Unbind() const = 0;

      virtual uint32_t GetCount() const = 0;

      static IndexBuffer* Create(uint32_t* indices, uint32_t count);
  };

}