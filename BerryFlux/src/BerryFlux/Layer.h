#pragma once

#include "BerryFlux/Core.h"
#include "BerryFlux/Events/Event.h"

namespace BerryFlux {
  class BERRYFLUX_API Layer {
    public:
      Layer(const std::string& name = "Layer");
      virtual ~Layer();

      //Engine Callbacks
      virtual void OnAttach() {}
      virtual void OnDetach() {}
      virtual void OnUpdate() {} //Called every frame.
      virtual void OnImGuiRender() {}
      virtual void OnEvent(Event& event) {} //whenever an event gets sent to the layer

      inline const std::string& GetName() const {return m_DebugName;}
    protected:
      std::string m_DebugName;
  };
}