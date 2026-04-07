#include "bfpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace BerryFlux {

  OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : m_Path(path)
  { 
    BF_CORE_WARN("Loading texture from: {0}", path);

    stbi_set_flip_vertically_on_load(1);
    int width, height, channels;
    stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    //Assert if the data is null, meaning the image failed to load
    if (!data)
    {
        BF_CORE_ERROR("Failed to load texture: {0}", path);
        BF_CORE_ASSERT(false, "Texture load failed!");
    }

    BF_CORE_WARN("Channels: {0}", channels);

    m_Width = width;
    m_Height = height;

    //When the texture is not one to one mapping, opengl has to shrink or elongate the texture to fit the shape of the object. This is called minification and magnification. We need to set the texture parameters for how opengl should handle this. We will set it to linear, which means it will use linear interpolation to shrink or elongate the texture.
    GLenum internalFormat = 0;
    GLenum dataFormat = 0;

    if (channels == 4)
    {
        internalFormat = GL_RGBA8;
        dataFormat = GL_RGBA;
    }
    else if (channels == 3)
    {
        internalFormat = GL_RGB8;
        dataFormat = GL_RGB;
    }
    else if (channels == 1) 
    {
        internalFormat = GL_R8;
        dataFormat = GL_RED;
    }

    BF_CORE_ASSERT(internalFormat && dataFormat, "Unsupported image format!");

    //Uploading buffer of data to GPU 
    glGenTextures(1, &m_RendererID); //Generate a texture and store the renderer ID in m_RendererID
    glBindTexture(GL_TEXTURE_2D, m_RendererID);

    //Set texture swizzle for 1 channel textures to output the red channel as the color and set alpha to 1. This is because when we have a single channel texture, we want to use that channel as the color and set the alpha to 1 so that it is fully opaque. This is useful for things like masks or heightmaps where we only care about one channel of data.
    if (channels == 1)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ONE);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //Set the texture parameters for minification and magnification filters to nearest

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //Uploading texture 
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internalFormat,
        m_Width,
        m_Height,
        0,
        dataFormat,
        GL_UNSIGNED_BYTE,
        data
    ); //Upload the texture data to the GPU with the width and height of the image

    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture

    stbi_image_free(data); //Free the image data from memory
  }

  OpenGLTexture2D::~OpenGLTexture2D() 
  {
    glDeleteTextures(1, &m_RendererID); //Delete the texture from the GPU when the texture object is destroyed
  }

  void OpenGLTexture2D::Bind(uint32_t slot) const 
  {
    //To bind more than one texture at once 
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_RendererID); //Bind the texture to the GPU so it can be used in the shader
  }

}