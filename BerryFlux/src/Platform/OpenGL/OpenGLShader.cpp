#include "bfpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace BerryFlux {

  OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc) {

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); // Create an empty vertex shader handle
    const GLchar *source = vertexSrc.c_str(); // Send the vertex shader source code to GL
    glShaderSource(vertexShader, 1, &source, 0);
    glCompileShader(vertexShader); // Compile the vertex shader

    GLint isCompiled = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);

    if(isCompiled == GL_FALSE)
    {
      GLint maxLength = 0;
      glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
      std::vector<GLchar> infoLog(maxLength);
      glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
      glDeleteShader(vertexShader);// Compile the vertex shader  
      BF_CORE_ERROR("{0}",infoLog.data());
      BF_CORE_ASSERT(false, "Vertex shader compilation failure!")
      return;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Create an empty fragment shader handle
    source = fragmentSrc.c_str(); // Send the fragment shader source code to GL
    glShaderSource(fragmentShader, 1, &source, 0);
    glCompileShader(fragmentShader); // Compile the fragment shader

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
      GLint maxLength = 0;
      glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
      std::vector<GLchar> infoLog(maxLength);
      glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);
      glDeleteShader(fragmentShader);
      glDeleteShader(vertexShader);
      BF_CORE_ERROR("{0}",infoLog.data());
      BF_CORE_ASSERT(false, "Fragment shader compilation failure!")
      return;
    }

    // Vertex and fragment shaders are successfully compiled. Now time to link them together into a program.
    // Get a program object.
    m_RendererID = glCreateProgram();
    GLuint program = m_RendererID;

    // Attach our shaders to our program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    // Link our program
    glLinkProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE)
    {
      GLint maxLength = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
      std::vector<GLchar> infoLog(maxLength);
      glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
      glDeleteProgram(program);
      glDeleteShader(vertexShader);
      glDeleteShader(fragmentShader);

      BF_CORE_ERROR("{0}",infoLog.data());
      BF_CORE_ASSERT(false, "Shader link failure!")
      return;
    }

    // Always detach shaders after a successful link.
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
  }

  OpenGLShader::~OpenGLShader() {
    glDeleteProgram(m_RendererID);
  }

  void OpenGLShader::Bind() const {
    glUseProgram(m_RendererID); //This is what we do before we want to render something with this shader 
  }

  void OpenGLShader::Unbind() const {
    glUseProgram(0);
  }

  void OpenGLShader::UploadUniformInt(const std::string& name, int value)
  {
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1i(location, value);
  }

  void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
  {
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1f(location, value);
  }

  void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
  {
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform2f(location, values.x, values.y);
  }

  void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
  {
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform3f(location, values.x, values.y, values.z);
  }

  void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values)
  {
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform4f(location, values.x, values.y, values.z, values.w); 
  }

  void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
  {
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
  }

  void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
  {
    GLint location = glGetUniformLocation(m_RendererID, name.c_str()); //gives u location of the u_ProjectionMatrix we specified in the shader
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix)); //1 matrix, transpose matrix, pointer to value
  }
  
}