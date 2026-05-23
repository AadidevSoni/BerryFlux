#include "bfpch.h"
#include "Renderer2D.h"
#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"
#include <glm/gtc/matrix_transform.hpp>

namespace BerryFlux {

  struct Renderer2DStorage
  {
    Ref<VertexArray> QuadVertexArray;
    Ref<Shader> FlatColorShader;
    Ref<Shader> TextureShader;
  };

  static Renderer2DStorage* s_Data;

  void Renderer2D::Init()
  {
    s_Data = new Renderer2DStorage();

    s_Data->QuadVertexArray = VertexArray::Create();

    float squareVertices [5 * 4] = {
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
      0.5f, -0.5f, 0.0f, 1.0f, 0.0f, //Bottom right corner has red color as red is 1 and green is 0
      0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
      -0.5f, 0.5f, 0.0f, 0.0f, 1.0f //If negative then 0 else positive and these are the 4 corners
    };

    Ref<VertexBuffer> squareVB;
    squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

    BufferLayout squareVBLayout = {
      {ShaderDataType::Float3, "aPos", true},
      {BerryFlux::ShaderDataType::Float2, "aTexCoord", true}
    };
    squareVB->SetLayout(squareVBLayout);
    s_Data->QuadVertexArray->AddVertexBuffer(squareVB);

    uint32_t squareIndices[6] = {0,1,2,2,3,0};
    Ref<IndexBuffer> squareIB;
    squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices)/sizeof(uint32_t))); //passing as number as it is count of indices not size in bytes
    s_Data->QuadVertexArray->SetIndexBuffer(squareIB);

    s_Data->FlatColorShader = Shader::Create("assets/shaders/FlatColor.glsl");
    s_Data->TextureShader = Shader::Create("assets/shaders/Texture.glsl");
    s_Data->TextureShader->Bind();
    s_Data->TextureShader->SetInt("u_Texture", 0); //texture slot is 0
  }

  void Renderer2D::Shutdown()
  {
    delete s_Data; 
  }

  void Renderer2D::BeginScene(const OrthographicCamera& camera)
  {
    s_Data->FlatColorShader->Bind(); //Bind the shader before setting the uniform
    s_Data->FlatColorShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

    s_Data->TextureShader->Bind(); //Bind the shader before setting the uniform
    s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
  }

  void Renderer2D::EndScene()
  {

  }

  //No Rotation No Texture

  void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
  {
    DrawQuad({position.x, position.y, 0.0f},size,color);
  }

  void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
  {
    s_Data->FlatColorShader->Bind(); //Making sure shader is bound
    s_Data->FlatColorShader->SetFloat4("u_Color", color);

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) //identity matrix that we start off with to position
      * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f}); 
    s_Data->FlatColorShader->SetMat4("u_Transform", transform);

    s_Data->QuadVertexArray->Bind();
    RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
  }

  //Rotation No Texture

  void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
  {
    DrawQuad({position.x, position.y, 0.0f},size,rotation,color);
  }

  void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
  {
    s_Data->FlatColorShader->Bind(); //Making sure shader is bound
    s_Data->FlatColorShader->SetFloat4("u_Color", color);

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) //identity matrix that we start off with to position
      * glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
      * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f}); 
    s_Data->FlatColorShader->SetMat4("u_Transform", transform);

    s_Data->QuadVertexArray->Bind();
    RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
  }

  //No Rotation Texture

  void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture)
  {
    DrawQuad({position.x, position.y, 0.0f},size,texture);
  }

  void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
  {
    s_Data->TextureShader->Bind(); 

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) //identity matrix that we start off with to position
      * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f}); 
    s_Data->TextureShader->SetMat4("u_Transform", transform); 

    texture->Bind();

    s_Data->QuadVertexArray->Bind();
    RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
  }

  //Rotation Texture

  void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture)
  {
    DrawQuad({position.x, position.y, 0.0f},size,rotation,texture);
  }

  void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture)
  {
    s_Data->TextureShader->Bind(); 

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) //identity matrix that we start off with to position
      * glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
      * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f}); 
    s_Data->TextureShader->SetMat4("u_Transform", transform); 

    texture->Bind();

    s_Data->QuadVertexArray->Bind();
    RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
  }

}