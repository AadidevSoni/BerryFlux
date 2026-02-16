#pragma once
//Header file

extern BerryFlux::Application* BerryFlux::CreateApplication();

int main(int argc, char** argv)
{

  BerryFlux::Log::Init();
  //BerryFlux::Log::GetCoreLogger()->warn("Initialized Core Log!");
  BF_CORE_WARN("Initialized Core Log!");
  //BerryFlux::Log::GetClientLogger()->info("Initialized Client Log!");
  int a = 10;
  BF_INFO("Initialized Client Log! Var={0}",10);

  auto app = BerryFlux::CreateApplication();
  app->Run();
  delete app;
}
