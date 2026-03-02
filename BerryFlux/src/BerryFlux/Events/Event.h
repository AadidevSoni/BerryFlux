#pragma once

#include "BerryFlux/Core.h"

#include <string>
#include <functional>

namespace BerryFlux {
  //Event is blocking i.e, when an event occurs, it immediately gets dispatched and is dealth with right there
  //No buffering is done for now

  enum class EventType { //code assigned to each event type
    None = 0,
    WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
  };

  //an event can go into multiple categories and that is why we are storing them in bits 
  enum EventCategory //like a keyboard and mouse event are input event
	{
		None = 0,
		EventCategoryApplication    = BIT(0),
		EventCategoryInput          = BIT(1),
		EventCategoryKeyboard       = BIT(2),
		EventCategoryMouse          = BIT(3),
		EventCategoryMouseButton    = BIT(4)
	};

  //MACROS which makes it easier to assign the EventType and Name for each of the events
  //non instance needed if we want to know the type of event just from a pointer to the event
#define EVENT_CLASS_TYPE(type) \
    static EventType GetStaticType() { return EventType::type; } \
    virtual EventType GetEventType() const override { return GetStaticType(); } \
    virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

  //Base class for events
  class BERRYFLUX_API Event {
    friend class EventDispatcher;

    public:
      virtual EventType GetEventType() const = 0;
      virtual const char* GetName() const = 0; //for debugging to get event name
      virtual int GetCategoryFlags() const = 0;
      virtual std::string ToString() const {return GetName();} //returns the name of the event. ToString is for debugging only

      inline bool IsInCategory(EventCategory category) { //utility function to filter events based on category if 0 then not in category else in category
        return GetCategoryFlags() & category;
      }
    protected:
      bool m_Handled = false; //To see if the event has been handled or not as later when dispatched through layers, we have to see if the event has been handled so that layer underneath it doesnt receive that event
  };

  //Dispatch events based on their type
  class EventDispatcher {
    template<typename T>
    using EventFn = std::function<bool(T&)>; //returns bool and takes T refernce whereT is any event type
    public:
      //When we get an event, we will receive it as a reference and it can be any event
      EventDispatcher(Event& event) : m_Event(event) {}
      template<typename T>
      bool Dispatch(EventFn<T> func) { //function that takes in window resize event and returns a boolean
        if(m_Event.GetEventType() == T::GetStaticType()) {
          m_Event.m_Handled = func(*(T*)&m_Event); //if the function matches, then we will call the event as we will go through every template or event type
          return true;
        }
        return false;
      }
    private:
      Event& m_Event;
  };

  //output string operator for the logging library so it can easily call ToString
  inline std::ostream& operator << (std::ostream& os, const Event& e) {
    return os << e.ToString();
  }

}