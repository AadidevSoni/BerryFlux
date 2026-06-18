#include "bfpch.h"
#include "Renderer2D.h"
#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"
#include <glm/gtc/matrix_transform.hpp>

namespace BerryFlux {

  struct QuadVertex 
  {
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 TexCoord;
  };

  struct Renderer2DStorage
  {
    //Max per draw call
    const uint32_t MaxQuads = 10000;
    const uint32_t MaxVertices = MaxQuads * 4;
    const uint32_t MaxIndices = MaxQuads * 6;

    Ref<VertexArray> QuadVertexArray;
    Ref<VertexBuffer> QuadVertexBuffer;
    Ref<Shader> TextureShader;
    Ref<Texture2D> WhiteTexture;

    uint32_t QuadIndexCount = 0;
    QuadVertex* QuadVertexBufferBase = nullptr;
    QuadVertex* QuadVertexBufferPtr = nullptr;
  };

  static Renderer2DStorage s_Data;

  void Renderer2D::Init()
  {
    BF_PROFILE_FUNCTION(); 

    s_Data.QuadVertexArray = VertexArray::Create();

    s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));

    BufferLayout quadVBLayout = {
      {ShaderDataType::Float3, "aPos", true},
      {ShaderDataType::Float4, "aColor", true},
      {ShaderDataType::Float2, "aTexCoord", true}
    };
    s_Data.QuadVertexBuffer->SetLayout(quadVBLayout);
    s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

    s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

    uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

    //Set indices
    uint32_t offset = 0;
    for(uint32_t i=0;i<s_Data.MaxIndices;i += 6) {
      quadIndices[i+0] = offset + 0;
      quadIndices[i+1] = offset + 1;
      quadIndices[i+2] = offset + 2;

      quadIndices[i+3] = offset + 2;
      quadIndices[i+4] = offset + 3;
      quadIndices[i+5] = offset + 0;

      offset += 4;
    }

    Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices,  s_Data.MaxIndices); //passing as number as it is count of indices not size in bytes
    s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
    delete[] quadIndices;

    s_Data.WhiteTexture = Texture2D::Create(1,1);
    uint32_t whiteTextureData = 0xffffffff;
    s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

    s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");
    s_Data.TextureShader->Bind();
    s_Data.TextureShader->SetInt("u_Texture", 0); //texture slot is 0
  }

  void Renderer2D::Shutdown()
  {
    BF_PROFILE_FUNCTION();
  }

  void Renderer2D::BeginScene(const OrthographicCamera& camera)
  {
    BF_PROFILE_FUNCTION();

    s_Data.TextureShader->Bind(); //Bind the shader before setting the uniform
    s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
    s_Data.QuadIndexCount = 0;
    s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
  }

  void Renderer2D::EndScene()
  {
    BF_PROFILE_FUNCTION();

    uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase; //we subtract and cast to uint8_t as it has one byte so we get the size instead of count of elements
    s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

    Flush();
  }

  void Renderer2D::Flush() 
  {
    RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
  }

  //No Rotation No Texture

  void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float tilingFactor)
  {
    BF_PROFILE_FUNCTION();

    DrawQuad({position.x, position.y, 0.0f},size,color,tilingFactor);
  }

  void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, float tilingFactor)
  {
    BF_PROFILE_FUNCTION();

    //4 times as 4 vertices
    s_Data.QuadVertexBufferPtr->Position = position;
    s_Data.QuadVertexBufferPtr->Color = color;
    s_Data.QuadVertexBufferPtr->TexCoord = {0.0f,0.0f};
    s_Data.QuadVertexBufferPtr++;

    //bottom right
    s_Data.QuadVertexBufferPtr->Position = {position.x + size.x,position.y,0.0f};
    s_Data.QuadVertexBufferPtr->Color = color;
    s_Data.QuadVertexBufferPtr->TexCoord = {1.0f,0.0f};
    s_Data.QuadVertexBufferPtr++;

    //top right
    s_Data.QuadVertexBufferPtr->Position = {position.x + size.x,position.y + size.y,0.0f};
    s_Data.QuadVertexBufferPtr->Color = color;
    s_Data.QuadVertexBufferPtr->TexCoord = {1.0f,1.0f};
    s_Data.QuadVertexBufferPtr++;

    //top left
    s_Data.QuadVertexBufferPtr->Position = {position.x,position.y + size.y,0.0f};
    s_Data.QuadVertexBufferPtr->Color = color;
    s_Data.QuadVertexBufferPtr->TexCoord = {0.0f,1.0f};
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadIndexCount += 6;

    /*s_Data.TextureShader->SetFloat("u_TilingFactor", tilingFactor); 
    //Bind white texture 
    s_Data.WhiteTexture->Bind();

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) //identity matrix that we start off with to position
      * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f}); 
    s_Data.TextureShader->SetMat4("u_Transform", transform);

    s_Data.QuadVertexArray->Bind();
    RenderCommand::DrawIndexed(s_Data.QuadVertexArray);*/
  }

  //Rotation No Texture

  void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color,float tilingFactor)
  {
    BF_PROFILE_FUNCTION();

    DrawQuad({position.x, position.y, 0.0f},size,rotation,color,tilingFactor);
  }

  void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color,float tilingFactor)
  {
    BF_PROFILE_FUNCTION();

    s_Data.TextureShader->SetFloat4("u_Color", color);
    s_Data.TextureShader->SetFloat("u_TilingFactor", tilingFactor); 
    //Bind white texture 
    s_Data.WhiteTexture->Bind();

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) //identity matrix that we start off with to position
      * glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
      * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f}); 
    s_Data.TextureShader->SetMat4("u_Transform", transform);

    s_Data.QuadVertexArray->Bind();
    RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
  }

  //No Rotation Texture

  void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor)
  {
    BF_PROFILE_FUNCTION();

    DrawQuad({position.x, position.y, 0.0f},size,texture,tilingFactor);
  }

  void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor)
  {
    BF_PROFILE_FUNCTION();

    s_Data.TextureShader->SetFloat4("u_Color", glm::vec4(1.0f)); //We still have to bind a color or else it will remain as the previous color used so we make it pure white which makes no change to the original texture.
    s_Data.TextureShader->SetFloat("u_TilingFactor", tilingFactor); 
    texture->Bind();

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) //identity matrix that we start off with to position
      * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f}); 
    s_Data.TextureShader->SetMat4("u_Transform", transform); 

    s_Data.QuadVertexArray->Bind(); 
    RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
  }

  void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color, float tilingFactor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, rotation, texture, color,tilingFactor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color, float tilingFactor)
	{
		s_Data.TextureShader->SetFloat4("u_Color", color);
    s_Data.TextureShader->SetFloat("u_TilingFactor", tilingFactor); 
		texture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data.TextureShader->SetMat4("u_Transform", transform);

		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
	}

}