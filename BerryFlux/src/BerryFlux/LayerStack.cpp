#include "bfpch.h"
#include "LayerStack.h"

namespace BerryFlux {
  LayerStack::LayerStack() {
    m_LayerInsert = m_Layers.begin();
  }

  LayerStack::~LayerStack() {
    for(Layer* layer: m_Layers) {
      delete layer;
    }
  }

  void LayerStack::PushLayer(Layer* layer) {
    m_LayerInsert = m_Layers.emplace(m_LayerInsert, layer); //Insert before overlays. emplace() inserts efficiently.
  }

  void LayerStack::PushOverlay(Layer* overlay) {
    m_Layers.emplace_back(overlay); //Adds overlay to the end.
  }

  void LayerStack::PopLayer(Layer* layer) {
    auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
    if(it != m_Layers.end()) { //If found.
      m_Layers.erase(it); //Remove from vector.
      m_LayerInsert--; //Move insert position back. Because one normal layer removed.
    }
  }

  //Overlays are always rendered at the last
  void LayerStack::PopOverlay(Layer* overlay) {
    auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
    if(it != m_Layers.end()) {
      m_Layers.erase(it);
    }
  }
}