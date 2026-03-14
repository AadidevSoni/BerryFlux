#pragma once 

namespace BerryFlux {

  class GraphicsContext {
    public:
      virtual void Init() = 0;
      virtual void SwapBuffers() = 0;
  };

}