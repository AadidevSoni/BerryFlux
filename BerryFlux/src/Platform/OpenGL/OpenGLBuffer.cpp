#include "bfpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace BerryFlux {

  OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size) 
  {
    BF_PROFILE_FUNCTION(); 

    glGenBuffers(1, &m_RendererID); //Combining the step og generating and binding buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW); //null for data pointer
  }

  OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size) 
  {
    BF_PROFILE_FUNCTION(); 

    glGenBuffers(1, &m_RendererID); //Combining the step og generating and binding buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
  }

  OpenGLVertexBuffer::~OpenGLVertexBuffer() 
  {
    BF_PROFILE_FUNCTION(); 

    glDeleteBuffers(1,&m_RendererID);
  }

  void OpenGLVertexBuffer::Bind() const 
  {
    BF_PROFILE_FUNCTION(); 

    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
  }

  void OpenGLVertexBuffer::Unbind() const 
  {
    BF_PROFILE_FUNCTION(); 

    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  void OpenGLVertexBuffer::SetData(const  void* data, uint32_t size)
  {
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data); //offset 0 of target buffer
  }

  //Index buffer
  OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count) : m_Count(count)
  {
    BF_PROFILE_FUNCTION(); 

    glGenBuffers(1, &m_RendererID); //Combining the step og generating and binding buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
  }

  OpenGLIndexBuffer::~OpenGLIndexBuffer() 
  {
    BF_PROFILE_FUNCTION(); 

    glDeleteBuffers(1,&m_RendererID);
  }

  void OpenGLIndexBuffer::Bind() const 
  {
    BF_PROFILE_FUNCTION(); 

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
  }

  void OpenGLIndexBuffer::Unbind() const 
  {
    BF_PROFILE_FUNCTION(); 
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
}