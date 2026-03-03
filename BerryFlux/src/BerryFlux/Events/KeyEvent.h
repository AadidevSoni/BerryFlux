#pragma once

#include "Event.h"

namespace BerryFlux {

  //Abstract event because we cant directly call this event as it has protected constructor
  class BERRYFLUX_API KeyEvent : public Event { //inherited from base event class
    public:
      inline int GetKeyCode() const {return m_KeyCode;} //getter inline: Small function → compiler can insert directly → faster.
      //macro
      EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput) 
    protected:
      KeyEvent(int keycode) //nothing else can contruct it
        : m_KeyCode(keycode) {}

      int m_KeyCode;
  };

  //Actual event that can call KeyEvent
  class BERRYFLUX_API KeyPressedEvent : public KeyEvent {
    public:
      KeyPressedEvent(int keycode, int repeatCount) : KeyEvent(keycode), m_RepeatCount(repeatCount) {}
      inline int GetRepeatCount() const {return m_RepeatCount;}
      std::string ToString() const override {
        std::stringstream ss;
        ss << "KeyPressedEvent: " << m_KeyCode << "(" << m_RepeatCount << "repeats)";
        return ss.str(); 
      }
      //Macro
      EVENT_CLASS_TYPE(KeyPressed)
    private:
      int m_RepeatCount;
  };

  class BERRYFLUX_API KeyReleasedEvent : public KeyEvent {
    public:
      KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}
      std::string ToString() const override {
        std::stringstream ss;
        ss << "KeyReleasedEvent: " << m_KeyCode;
        return ss.str();
      }

      EVENT_CLASS_TYPE(KeyReleased)
  };

}