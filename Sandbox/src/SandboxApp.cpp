#include <BerryFlux.h>

class Sandbox : public BerryFlux::Application {
  public:
    Sandbox() {

    }

    ~Sandbox() {

    }
};
//This overrides the engine factory. So EntryPoint doesn’t know about Sandbox.
BerryFlux::Application* BerryFlux::CreateApplication() {
  return new Sandbox();
}