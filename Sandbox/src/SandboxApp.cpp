#include <BerryFlux.h>

class ExampleLayer : public BerryFlux::Layer {
  public:
    ExampleLayer():Layer("Example") {

    }

    void OnUpdate() override {
      BF_INFO("Example Layer::Update");
    }

    void OnEvent(BerryFlux::Event& event) override {
      BF_TRACE("{0}", event.ToString());
    }
};

class Sandbox : public BerryFlux::Application {
  public:
    Sandbox() {
      PushLayer(new ExampleLayer());
    }

    ~Sandbox() {

    }
};
//This overrides the engine factory. So EntryPoint doesn’t know about Sandbox.
BerryFlux::Application* BerryFlux::CreateApplication() {
  return new Sandbox();
}