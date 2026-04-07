#include "bfpch.h"
#include "OpenGLShader.h"

#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace BerryFlux {

  static GLenum ShaderTypeFromString(const std::string& type) 
  {
    if(type == "vertex")
      return GL_VERTEX_SHADER;
    if(type == "fragment" || type == "pixel")
      return GL_FRAGMENT_SHADER;

    BF_CORE_ASSERT(false, "Unknown shader type!");
    return 0;
  }

  OpenGLShader::OpenGLShader(const std::string& filepath) 
  {
    std::string shaderSource = ReadFile(filepath);
    //We need to separate the vertex and fragment shader source code from the shader file. We will use a simple convention where we will have a line that says #shader vertex and #shader fragment to separate the two shader source codes. We will use a stringstream to read the shader source code and separate it based on the #shader lines.
    auto shaderSources = PreProcess(shaderSource);
    Compile(shaderSources);
  }

  OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc) 
  {
    std::unordered_map<GLenum, std::string> sources;
    sources[GL_VERTEX_SHADER] = vertexSrc;
    sources[GL_FRAGMENT_SHADER] = fragmentSrc;
    Compile(sources);
  }

  OpenGLShader::~OpenGLShader() {
    glDeleteProgram(m_RendererID);
  }

  std::string OpenGLShader::ReadFile(const std::string& filepath)
  {
    std::string result;
    std::ifstream in(filepath, std::ios::in | std::ios::binary);
    if(in) 
    {
      in.seekg(0, std::ios::end); //Goes to the end of the file
      result.resize(in.tellg()); //Resizes the result string to the size of the file
      in.seekg(0, std::ios::beg); //Goes back to the beginning of the file
      in.read(&result[0], result.size()); //Reads the file into the result string
      in.close();
    }else 
    {
      BF_CORE_ERROR("Could not open file: {0}", filepath);
      BF_CORE_ASSERT(false, "File open failure!");
    }
    return result; 
  }

  std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source) 
  {
    std::unordered_map<GLenum, std::string> shaderSources;

    const char* typeToken = "#type";
    size_t typeTokenLength = strlen(typeToken);
    size_t pos = source.find(typeToken, 0); //Start of shader type declaration  
    while(pos != std::string::npos) 
    {
      size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
      BF_CORE_ASSERT(eol != std::string::npos, "Syntax error");
      size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
      std::string type = source.substr(begin, eol - begin); //Extract shader type name
      BF_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

      size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
      BF_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
      pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

      shaderSources[ShaderTypeFromString(type)] = 
        (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos); //Extract shader code
    }
    return shaderSources;
  }

  void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
  {
    GLuint program = glCreateProgram();
    std::vector<GLenum> glShaderIDs(shaderSources.size());
    //Compiles both vbertex and fragment shaders and links them together into a shader program. Then we can use that shader program to render our objects. We will use the OpenGL functions glCreateShader, glShaderSource, glCompileShader, glGetShaderiv, glGetShaderInfoLog, glCreateProgram, glAttachShader, glLinkProgram, glGetProgramiv, glGetProgramInfoLog, glDeleteShader, glDeleteProgram to do this.
    for(auto& kv : shaderSources) 
    {
      GLenum type = kv.first;
      const std::string source = kv.second;

      GLuint shader = glCreateShader(type); // Create an empty vertex shader handle

      const GLchar *sourceCstr = source.c_str(); // Send the vertex shader source code to GL
      glShaderSource(shader, 1, &sourceCstr, 0);
      glCompileShader(shader); // Compile the vertex shader

      GLint isCompiled = 0;
      glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

      if(isCompiled == GL_FALSE)
      {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
        glDeleteShader(shader);// Compile the vertex shader  
        BF_CORE_ERROR("{0}",infoLog.data());
        BF_CORE_ASSERT(false, "Shader compilation failure!")
        break;
      }
      glAttachShader(program, shader); //Attach the compiled shader to the program
      glShaderIDs.push_back(shader);
    }

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

      for(auto id : glShaderIDs){
        glDeleteShader(id);
      }

      BF_CORE_ERROR("{0}",infoLog.data());
      BF_CORE_ASSERT(false, "Shader link failure!")
      return;
    }

    // Always detach shaders after a successful link.
    for(auto id : glShaderIDs){
      glDetachShader(program, id);
    }

    m_RendererID = program;
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