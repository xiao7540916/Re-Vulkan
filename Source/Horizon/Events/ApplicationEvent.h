#pragma once

#include "Horizon/Events/Event.h"

namespace Horizon
{
	class ApplicationUpdateEvent : public Event
	{
	public:
		ApplicationUpdateEvent() = default;
		EVENT_TYPE(EventType::ApplicationUpdate)
		EVENT_CATEGORY_FLAGS(EventCategoryFlags::Application)
	};

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(uint32 width, uint32 height) : mWidth(width), mHeight(height) {}
		uint32 GetWidth() const { return mWidth; }
		uint32 GetHeight() const { return mHeight; }
		EVENT_TYPE(EventType::WindowResize)
		EVENT_CATEGORY_FLAGS(EventCategoryFlags::Window)
	private:
		uint32 mWidth, mHeight;
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;
		EVENT_TYPE(EventType::WindowClose)
		EVENT_CATEGORY_FLAGS(EventCategoryFlags::Window)
	};
}
