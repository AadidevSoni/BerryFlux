#pragma once

#include "BerryFlux/Layer.h"

namespace BerryFlux {

  class BERRYFLUX_API ImGuiLayer : public Layer{
    public:
      ImGuiLayer();
      ~ImGuiLayer();

      virtual void OnAttach() override;
      virtual void OnDetach() override;
      virtual void OnImGuiRender() override;

      //For layers to do their own ImGui Rendering. Begin and end window renderign by different layers 
      void Begin();
      void End();
    private:
      float m_Time = 0.0f;
  };
}