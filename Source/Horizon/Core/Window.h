#pragma once

#include <GLFW/glfw3.h>

#include "Horizon/Core/HorizonCommon.h"
#include "Horizon/Math/HorizonMath.h"
#include "Horizon/Events/Events.h"

namespace Horizon
{
	enum class WindowMode
	{
		Normal,
		Minimized,
		Fullscreen,
	};

	struct WindowCreateInfo
	{
		uint32 width = 1920;
		uint32 height = 1080;
		bool resizable = true;
		String title = "Horizon";
		WindowMode mode = WindowMode::Normal;
	};

	class Window
	{
	public:
		using EventCallback = std::function<void(Event&)>;
	public:
		static Window* Create(const WindowCreateInfo* pCreateInfo);
		virtual ~Window();
		void PollEvents();
		void Resize(uint32 width, uint32 height);
		uint32 GetWidth() const { return mWidth; }
		uint32 GetHeight() const { return mHeight; }
		WindowMode GetMode() const { return mMode; }
		GLFWwindow* GetGLFWwindow() const { return mGLFWwindow; }
		void SetWindowPos(int32 x, int32 y);
		void SetWindowTitle(const String& title);
		void SetEventCallback(const EventCallback& callback) { mEventCallback = callback; }
	private:
		Window(uint32 width, uint32 height, WindowMode initialWindowMode);
		void UpdateWindowSize_Internal();
		void SetWindowSize_Internal(uint32 width, uint32 height);
		uint32 mWidth;
		uint32 mHeight;
		Vector2 mMousePosRatio;
		// TODO: Keep tracking of current window mode. Set to fullscreen etc.
		WindowMode mMode;
		GLFWwindow* mGLFWwindow;
		EventCallback mEventCallback;
	};
}
