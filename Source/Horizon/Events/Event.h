#pragma once

#include "Horizon/Core/HorizonCommon.h"

namespace Horizon
{
	enum class EventType
	{
		None = 0,
		WindowClose, 
		WindowResize,
		WindowFocus,
		WindowLostFocus,
		WindowMove,
		ApplicationUpdate,
		KeyPresse,
		KeyRelease,
		MouseButtonPresse,
		MouseButtonRelease,
		MouseMove,
		MouseScroll,
	};

	enum class EventCategoryFlags : uint32
	{
		None = 0,
		Input = 0x1,
		Mouse = 0x2,
		Keyboard = 0x4,
		Window = 0x8,
		Application = 0x10,
	};
	HORIZON_ENUM_CLASS_OPERATORS(EventCategoryFlags);

#define EVENT_TYPE(type) static EventType GetStaticType() { return type; } virtual EventType GetType() const override { return GetStaticType(); }
#define EVENT_CATEGORY_FLAGS(flags) virtual EventCategoryFlags GetCategoryFlags() const override { return flags; }

	class Event
	{
	public:
		Event() = default;
		virtual ~Event() = default;
		bool mHandled = false;
		bool IsHandled() const { return mHandled; }
		virtual EventType GetType() const = 0;
		virtual EventCategoryFlags GetCategoryFlags() const = 0;
		bool IsInCategory(EventCategoryFlags category) { return (GetCategoryFlags() & category) != (EventCategoryFlags)0; }
	private:
		
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event) : mEvent(event) {}
		// F will be deduced by the compiler
		template<typename T, typename F>
		bool Dispatch(const F& func)
		{
			if (mEvent.GetType() == T::GetStaticType())
			{
				mEvent.mHandled |= func(static_cast<T&>(mEvent));
				return true;
			}
			return false;
		}
	private:
		Event& mEvent;
	};
}
