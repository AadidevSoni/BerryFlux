#pragma once
//Header file


//extern - The symbol is defined in another translation unit
extern BerryFlux::Application* BerryFlux::CreateApplication(); //Trust me — this function exists somewhere. You will find it during linking.

int main(int argc, char** argv)
{

  BerryFlux::Log::Init(); //Engine boot sequence.
  //BerryFlux::Log::GetCoreLogger()->warn("Initialized Core Log!");
  BF_CORE_WARN("Initialized Core Log!");
  //BerryFlux::Log::GetClientLogger()->info("Initialized Client Log!");
  int a = 10;
  BF_INFO("Initialized Client Log! Var={0}",10);

  BF_PROFILE_BEGIN_SESSION("Startup", "../Sandbox/BerryFluxProfile-Startup.json");
  auto app = BerryFlux::CreateApplication();
  BF_PROFILE_END_SESSION();

  BF_PROFILE_BEGIN_SESSION("Runtime", "../Sandbox/BerryFluxProfile-Runtime.json");
  app->Run();
  BF_PROFILE_END_SESSION();

  BF_PROFILE_BEGIN_SESSION("Shutdown", "../Sandbox/BerryFluxProfile-Shutdown.json");;
  delete app;
  BF_PROFILE_END_SESSION();
}
