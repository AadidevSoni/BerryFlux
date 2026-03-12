#pragma once

#include "BerryFlux/Core.h"
#include "Layer.h"
#include <vector>

namespace BerryFlux {
  class BERRYFLUX_API LayerStack { //It is not a stack as we can push things in the middle too
    public:
      LayerStack();
      ~LayerStack();

      void PushLayer(Layer* layer);
      void PushOverlay(Layer* overlay); //Overlay layers: ImGuiLayer, DebugLayer
      void PopLayer(Layer* layer);
      void PopOverlay(Layer* overlay);

      //Allows: for(Layer* layer : layerStack) Used every frame.
      //Range based for loop using begin and end
      std::vector<Layer*>::iterator begin() {return m_Layers.begin();}
      std::vector<Layer*>::iterator end() {return m_Layers.end();}
    private:
      //vector because every frame, this will be iterated over. 
      //Layerstack is a wrapper around layers
      std::vector<Layer*> m_Layers; //Stores all layers
      // Old implementation std::vector<Layer*>::iterator m_LayerInsert; //Marks where normal layers end and overlay starts as overlays always at end
      unsigned int m_LayerInsertIndex = 0;
  };
}
