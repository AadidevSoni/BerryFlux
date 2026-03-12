#include <BerryFlux.h>

class ExampleLayer : public BerryFlux::Layer {
  public:
    ExampleLayer():Layer("Example") {

    }

    void OnUpdate() override {
      //BF_INFO("Example Layer::Update");

      if(BerryFlux::Input::IsKeyPressed(BF_KEY_TAB)) {
        BF_INFO("Tab key is pressed (poll)");
      }
    }

    void OnEvent(BerryFlux::Event& event) override {
      //BF_TRACE("{0}", event.ToString());
      if(event.GetEventType() == BerryFlux::EventType::KeyPressed) {
        BerryFlux::KeyPressedEvent& e = (BerryFlux::KeyPressedEvent&)event;
        BF_TRACE("{0}", (char)e.GetKeyCode());
        if(e.GetKeyCode() == BF_KEY_TAB) {
          BF_INFO("Tab key is pressed (event)");
        }
      }
    }
};

class Sandbox : public BerryFlux::Application {
  public:
    Sandbox() {
      PushLayer(new ExampleLayer());
      PushOverlay(new BerryFlux::ImGuiLayer());
    }

    ~Sandbox() {

    }
};
//This overrides the engine factory. So EntryPoint doesn’t know about Sandbox.
BerryFlux::Application* BerryFlux::CreateApplication() {
  return new Sandbox();
}