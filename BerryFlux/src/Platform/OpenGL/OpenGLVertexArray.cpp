#include "bfpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace BerryFlux {

  static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
  {
    switch(type) 
    {
      case BerryFlux::ShaderDataType::Float:     return GL_FLOAT;
      case BerryFlux::ShaderDataType::Float2:    return GL_FLOAT;
      case BerryFlux::ShaderDataType::Float3:    return GL_FLOAT;
      case BerryFlux::ShaderDataType::Float4:    return GL_FLOAT;
      case BerryFlux::ShaderDataType::Mat3:      return GL_FLOAT;
      case BerryFlux::ShaderDataType::Mat4:      return GL_FLOAT;
      case BerryFlux::ShaderDataType::Int:       return GL_INT;
      case BerryFlux::ShaderDataType::Int2:      return GL_INT;
      case BerryFlux::ShaderDataType::Int3:      return GL_INT;
      case BerryFlux::ShaderDataType::Int4:      return GL_INT;
      case BerryFlux::ShaderDataType::Bool:      return GL_BOOL;
      case ShaderDataType::None: break;
    }
    BF_CORE_ASSERT(false, "Unknow shader data type!");
    return 0;
  }

  OpenGLVertexArray::OpenGLVertexArray()
  {
    glGenVertexArrays(1, &m_RendererID);
  }

  OpenGLVertexArray::~OpenGLVertexArray()
  {
    glDeleteVertexArrays(1, &m_RendererID);
  }

  void OpenGLVertexArray::Bind() const 
  {
    glBindVertexArray(m_RendererID);
  }

  void OpenGLVertexArray::Unbind() const 
  {
    glBindVertexArray(0);
  }

  void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
  {
    BF_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex buffer has no layout!");

    glBindVertexArray(m_RendererID);
    vertexBuffer->Bind();

    uint32_t index = 0;
    const auto& layout = vertexBuffer->GetLayout();
    for(const auto& element:layout)
    {
      //So that GPU can read the data
      glEnableVertexAttribArray(index); //creating index 
      glVertexAttribPointer(index, 
        element.GetComponentCount(), 
        ShaderDataTypeToOpenGLBaseType(element.Type), 
        element.Normalized ? GL_TRUE : GL_FALSE, 
        layout.GetStride(), 
        (const void*)element.Offset
      ); //Vertex attributes are stored in vertex array
      index++;
    }

    m_VertexBuffers.push_back(vertexBuffer);
  }

  void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
  {
    glBindVertexArray(m_RendererID);
    indexBuffer->Bind();

    m_IndexBuffer = indexBuffer;
  }

}