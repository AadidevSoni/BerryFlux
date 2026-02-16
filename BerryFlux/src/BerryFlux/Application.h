#pragma once 

#include "Core.h"

namespace BerryFlux {
  class BERRYFLUX_API Application
  {
    public:
      Application();
      virtual ~Application();

      void Run();
  };

  //To be defined in Client
  Application* CreateApplication();
}