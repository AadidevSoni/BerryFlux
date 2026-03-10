#pragma once

#include "BerryFlux/Layer.h"

namespace BerryFlux {

  class BERRYFLUX_API ImGuiLayer : public Layer{
    public:
      ImGuiLayer();
      ~ImGuiLayer();

      void OnAttach();
      void OnDetach();
      void OnUpdate();
      void OnEvent(Event& event);
    private:
      float m_Time = 0.0f;
  };
}