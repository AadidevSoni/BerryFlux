#pragma once 
#include "bfpch.h"
#include "BerryFlux/Core.h"
#include "BerryFlux/Events/Event.h"

namespace BerryFlux {
  //Abstract representation of a window and is platform independent 
  //Engine directly interacts with this interface to create window as we dont want engine to directly know  and work with GLFW
  struct WindowProps {
    std::string Title;
    unsigned int Width;
    unsigned int Height;

    //string& for perfomance reasons so that the string doesnt get copied and used each time constructor is called 
    //: Title(title), Width(width), Height(height) is called an initializer list.
    WindowProps(const std::string& title = "BerryFlux Engine", unsigned int width = 1280, unsigned int height = 720) 
    : Title(title), Width(width), Height(height) {}
  };

  class BERRYFLUX_API Window {
    public:
      //This creates a type alias - EventCallbackFn is another name for std::function<void(Event&)
      //std::function is a wrapper for any callable object.
      //Function signature - void SomeFunction(Event& e)
      using EventCallbackFn = std::function<void(Event&)>;
      virtual ~Window() {}

      //Interfaces as this has to be implemented per platform
      //virtual enables runtime polymorphism, meaning the correct function runs based on the actual object type. that is GLFWWindow
      //That means when you call a function through a base-class pointer, C++ decides at runtime which implementation to use.
      virtual void OnUpdate() = 0;

      //=0 because Window does NOT implement this function. Instead, derived classes must implement it.
      virtual unsigned int GetWidth() const = 0;
      virtual unsigned int GetHeight() const = 0;

      //Window attributes
      //Every platform window implementation must provide a way to set an event callback.
      //This function tells the window: When an event happens, call THIS function. So the window doesn't process events itself. Instead it forwards them to the engine.
      //parameter is a function that takes Event& and returns nothing
      virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
      virtual void SetVSync(bool enabled) = 0;
      virtual bool IsVSync() const = 0;

      //Has to be implemented per platform
      //static here means: This function belongs to the class, not to an object. So you do not need a Window instance to call it.
      static Window* Create(const WindowProps& props = WindowProps());
      //WindowProps() is a default parameter.
      //function returns a GLFWWindow, but it is returned as a Window pointer.
  };
}