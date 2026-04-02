#pragma once

#include <string>
#include "BerryFlux/Core.h"

namespace BerryFlux {
  
  class Texture 
  {
    public:
      virtual ~Texture() = default;

      virtual uint32_t GetWidth() const = 0;
      virtual uint32_t GetHeight() const = 0;

      virtual void Bind(uint32_t slot = 0) const = 0;
  };

  //Actual texture class that we will use in the application code, this is the one we will create instances of and use in our application code, this inherits from the base texture class
  class Texture2D : public Texture 
  {
    public:
      static Ref<Texture2D> Create(const std::string& path); //File path to the texture image
  };

}