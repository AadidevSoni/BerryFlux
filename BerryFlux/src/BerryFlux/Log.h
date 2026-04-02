#include "bfpch.h"
#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace BerryFlux 
{
  class BERRYFLUX_API Log 
  {
    public:
      static void Init();

      inline static BerryFlux::Ref<spdlog::logger>& GetCoreLogger() {
        return s_CoreLogger;
      }

      inline static BerryFlux::Ref<spdlog::logger>& GetClientLogger() {
        return s_ClientLogger;
      }
    
    private:
      static BerryFlux::Ref<spdlog::logger> s_CoreLogger;
      static BerryFlux::Ref<spdlog::logger> s_ClientLogger;
  };
}

//Core Log macros
//... is variable amount of arguments
#define BF_CORE_ERROR(...)  ::BerryFlux::Log::GetCoreLogger()->error(__VA_ARGS__)
#define BF_CORE_WARN(...)   ::BerryFlux::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define BF_CORE_INFO(...)   ::BerryFlux::Log::GetCoreLogger()->info(__VA_ARGS__)
#define BF_CORE_TRACE(...)  ::BerryFlux::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define BF_CORE_FATAL(...)  ::BerryFlux::Log::GetCoreLogger()->fatal(__VA_ARGS__)

//Client Log macros
#define BF_ERROR(...)       ::BerryFlux::Log::GetClientLogger()->error(__VA_ARGS__)
#define BF_WARN(...)        ::BerryFlux::Log::GetClientLogger()->warn(__VA_ARGS__)
#define BF_INFO(...)        ::BerryFlux::Log::GetClientLogger()->info(__VA_ARGS__)
#define BF_TRACE(...)       ::BerryFlux::Log::GetClientLogger()->trace(__VA_ARGS__)
#define BF_FATAL(...)       ::BerryFlux::Log::GetClientLogger()->fatal(__VA_ARGS__)