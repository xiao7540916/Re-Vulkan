#pragma once

#include "Horizon/Input/Input.h"
#include "Horizon/Events/Event.h"
#include "Horizon/Math/HorizonMath.h"

namespace Horizon
{
	class MouseEvent : public Event
	{
	public:
		MouseEvent(const Vector2& normalizedPos, const Vector2& screenPos) : mNormalizedPos(normalizedPos), mScreenPos(screenPos) {}
		virtual Vector2 GetScreenPos() const { return mScreenPos; }
		virtual Vector2 GetNormalizedPos() const { return mNormalizedPos; }
	protected:
		Vector2 mScreenPos;
		Vector2 mNormalizedPos;
	};

	class MouseMoveEvent : public MouseEvent
	{
	public:
		MouseMoveEvent(const Vector2& normalizedPos, const Vector2& screenPos) : MouseEvent(normalizedPos, screenPos) {}
		EVENT_TYPE(EventType::MouseMove)
		EVENT_CATEGORY_FLAGS(EventCategoryFlags::Mouse | EventCategoryFlags::Input)
	};

	class MouseScrollEvent : public MouseEvent
	{
	public:
		MouseScrollEvent(const Vector2& offset, const Vector2& normalizedPos, const Vector2& screenPos) : mOffset(offset), MouseEvent(normalizedPos, screenPos) {}
		Vector2 GetOffset() const { return mOffset; }
		EVENT_TYPE(EventType::MouseScroll)
		EVENT_CATEGORY_FLAGS(EventCategoryFlags::Mouse | EventCategoryFlags::Input)
	private:
		Vector2 mOffset;
	};

	class MouseButtonEvent : public MouseEvent
	{
	public:
		MouseButtonID GetMouseButtonID() const { return mMouseButtonID; }
		EVENT_CATEGORY_FLAGS(EventCategoryFlags::Mouse | EventCategoryFlags::Input)
	protected:
		MouseButtonEvent(MouseButtonID mouseButtonID, const Vector2& normalizedPos, const Vector2& screenPos) : mMouseButtonID(mouseButtonID), MouseEvent(normalizedPos, screenPos) {}
		MouseButtonID mMouseButtonID;
	};

	class MouseButtonPressEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressEvent(MouseButtonID mouseButtonID, const Vector2& normalizedPos, const Vector2& screenPos) : MouseButtonEvent(mouseButtonID, normalizedPos, screenPos) {}
		EVENT_TYPE(EventType::MouseButtonPresse)
	};

	class MouseButtonReleaseEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleaseEvent(MouseButtonID mouseButtonID, const Vector2& normalizedPos, const Vector2& screenPos) : MouseButtonEvent(mouseButtonID, normalizedPos, screenPos) {}
		EVENT_TYPE(EventType::MouseButtonRelease)
	};
}
