#include "bfpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace BerryFlux { 

  Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size) //Decide which API we using and which class type to return 
  {
    switch(Renderer::GetAPI()) 
    {
      case RendererAPI::API::None:      BF_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
      case RendererAPI::API::OpenGL:    return CreateRef<OpenGLVertexBuffer>(vertices, size);
    }

    BF_CORE_ASSERT(false, "Unknown Renderer API!");
    return nullptr;
  }

  Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
  {
     switch(Renderer::GetAPI()) 
     {
      case RendererAPI::API::None:      BF_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
      case RendererAPI::API::OpenGL:    return CreateRef<OpenGLVertexBuffer>(size);
    }

    BF_CORE_ASSERT(false, "Unknown Renderer API!");
    return nullptr;
  }

  Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size) 
  {
    switch(Renderer::GetAPI()) 
    {
      case RendererAPI::API::None:      BF_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
      case RendererAPI::API::OpenGL:    return CreateRef<OpenGLIndexBuffer>(indices, size);
    }

    BF_CORE_ASSERT(false, "Unknown Renderer API!");
    return nullptr;
  }
}