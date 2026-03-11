#include "bfpch.h"
#include "WindowsInput.h"

#include <GLFW/glfw3.h>
#include "BerryFlux/Application.h"

namespace BerryFlux {

  Input* Input::s_Instance = new WindowsInput(); //We can initialize it here itself as there is no data 

  //It asks GLFW if the key is pressed and returns the boolean 
  bool WindowsInput::IsKeyPressedImpl(int keycode) 
  {
    //Static casting to convert from void pointer to glfw window pointer
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()); //Gets the window reference then get the glfw window
    //It tells whether a key is down or not
    auto state = glfwGetKey(window,keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
  }

  bool WindowsInput::IsMouseButtonPressedImpl(int button)
  {
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetMouseButton(window, button);
    return state == GLFW_PRESS;
  }

  std::pair<float, float> WindowsInput::GetMousePositionImpl() 
  {
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    return {(float)xpos, (float)ypos};
  }

  float WindowsInput::GetMouseXImpl()
  {
    auto[x,y] = GetMousePositionImpl();

    return x;
  }

  float WindowsInput::GetMouseYImpl()
  {
    auto[x,y] = GetMousePositionImpl();

    return y;
  }
}