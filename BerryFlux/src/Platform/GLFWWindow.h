#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "BerryFlux/Window.h"

struct GLFWwinddw;

namespace BerryFlux {
  //Inherits from Window
  class GLFWWindow : public Window {
    public:
      GLFWWindow(const WindowProps& props);
      virtual ~GLFWWindow();

      //override tells the compiler: “This function is meant to override a virtual function from the base class.”
      void OnUpdate() override;

      unsigned int GetWidth() const override { return m_Data.Width; }
      unsigned int GetHeight() const override { return m_Data.Height; }

      void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
      //VSync synchronizes rendering with the monitor refresh rate.
      void SetVSync(bool enabled) override;
      bool IsVSync() const override;

      //This function can be used to get the private glfw window pointer to other parts of the code
      inline virtual void* GetNativeWindow() const {return m_Window;}

    private:
      virtual void Init(const WindowProps& props);
      virtual void Shutdown();

    private:
      //This is the actual window created by GLFW.
      GLFWwindow* m_Window;

      //Store actual data that might be requested by GLFW during event callback
      struct WindowData {
        std::string Title;
        unsigned int Width, Height;
        bool VSync;

        EventCallbackFn EventCallback;
      };

      //This stores the current state of the window.
      WindowData m_Data;
  };

}