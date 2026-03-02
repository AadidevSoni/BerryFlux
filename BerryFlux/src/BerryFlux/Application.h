#pragma once 

#include "Core.h"
#include "Events/Event.h"

namespace BerryFlux {
  class BERRYFLUX_API Application //“Make this class visible outside the DLL” Without this, Sandbox couldn’t access engine classes.
  {
    //Every game inherits from this.
    public:
      Application();
      virtual ~Application(); //Required for inheritance safety.

      void Run();
  };

  //To be defined in Client
  Application* CreateApplication(); //Factory Pattern
}