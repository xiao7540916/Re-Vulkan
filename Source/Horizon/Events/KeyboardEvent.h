#pragma once

#include "Horizon/Input/Input.h"
#include "Horizon/Events/Event.h"

namespace Horizon
{
	class KeyboardEvent : public Event
	{
	public:
		KeyCode GetKeyCode() const { return mKeyCode; }
		EVENT_CATEGORY_FLAGS(EventCategoryFlags::Keyboard | EventCategoryFlags::Input)
	protected:
		KeyboardEvent(const KeyCode keyCode) : mKeyCode(keyCode) {}
		KeyCode mKeyCode;
	};

	class KeyPressEvent : public KeyboardEvent
	{
	public:
		KeyPressEvent(const KeyCode keyCode, const uint16 repeatCount) : KeyboardEvent(keyCode), mRepeatCount(repeatCount) {}
		uint16 GetRepeatCount() const { return mRepeatCount; }
		EVENT_TYPE(EventType::KeyPresse)
	private:
		uint16 mRepeatCount;
	};

	class KeyReleaseEvent : public KeyboardEvent
	{
	public:
		KeyReleaseEvent(const KeyCode keyCode) : KeyboardEvent(keyCode) {}
		EVENT_TYPE(EventType::KeyRelease)
	};
}
