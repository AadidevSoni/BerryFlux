#pragma once 

#include "BerryFlux/Renderer/Shader.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace BerryFlux {
  
  class OpenGLShader : public Shader {
    public:
      OpenGLShader(const std::string& filepath);
      OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
      virtual ~OpenGLShader();

      void Bind() const override;
      void Unbind() const override;

      void UploadUniformInt(const std::string& name, int  values);

      void UploadUniformFloat(const std::string& name, float values);
      void UploadUniformFloat2(const std::string& name, const glm::vec2& values);
      void UploadUniformFloat3(const std::string& name, const glm::vec3& values);
      void UploadUniformFloat4(const std::string& name, const glm::vec4& values);

      void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
      void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

    private:
      std::string ReadFile(const std::string& filepath);
      std::unordered_map<GLenum, std::string> PreProcess(const std::string& source); //GLEnum is the type of shader, string is the source code for that shader
      void Compile(const std::unordered_map<GLenum, std::string>& shaderSources); //This will compile the shader source code and link it to create a shader program
    private:
      uint32_t m_RendererID;
  };

}