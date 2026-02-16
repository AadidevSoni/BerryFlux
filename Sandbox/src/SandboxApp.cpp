#include <BerryFlux.h>

class Sandbox : public BerryFlux::Application {
  public:
    Sandbox() {

    }

    ~Sandbox() {

    }
};

BerryFlux::Application* BerryFlux::CreateApplication() {
  return new Sandbox();
}