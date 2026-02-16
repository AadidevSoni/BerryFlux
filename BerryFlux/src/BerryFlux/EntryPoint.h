#pragma once
//Header file

extern BerryFlux::Application* BerryFlux::CreateApplication();

int main(int argc, char** argv)
{
  printf("BerryFlux Engine Starting\n");
  auto app = BerryFlux::CreateApplication();
  app->Run();
  printf("BerryFlux Engine Closing\n");
  delete app;
}
