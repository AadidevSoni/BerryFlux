#include "bfpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace BerryFlux {
  
  Ref<Texture2D> Texture2D::Create(const std::string& path)
  {
    //We will implement this function in the OpenGLTexture2D class, so we will just call that function here and return it
    //This is the function that we will call in our application code and it will create an instance of the OpenGLTexture2D class and return it as a reference to a Texture2D object
    switch(Renderer::GetAPI()) {
      case RendererAPI::API::None:      BF_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
      case RendererAPI::API::OpenGL:    return std::make_shared<OpenGLTexture2D>(path);
    }

    BF_CORE_ASSERT(false, "Unknown Renderer API!");
    return nullptr;
  }

}