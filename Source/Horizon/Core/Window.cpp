#include "Horizon/Core/Window.h"
#include "Horizon/Log/LogManager.h"

namespace Horizon
{
	Window::Window(uint32 width, uint32 height, WindowMode initialWindowMode)
		: mWidth(width)
		, mHeight(height)
		, mMode(initialWindowMode)
		, mEventCallback(nullptr)
		, mGLFWwindow(nullptr)
		, mMousePosRatio(Vector2(1.0f / (float)width, 1.0f / (float)height))
	{

	}

	Window::~Window()
	{
		assert(mGLFWwindow != nullptr);
		glfwDestroyWindow(mGLFWwindow);
		glfwTerminate();
		mGLFWwindow = nullptr;
	}

	static void ErrorCallback(int errorCode, const char* description)
	{
		LOG_ERROR("GLFW error occurs. [error code]: {}, [desctiption]: {}.", errorCode, description);
	}

	Window* Window::Create(const WindowCreateInfo* pCreateInfo)
	{
		// Error callback function may be called before glfwInit.
		glfwSetErrorCallback(ErrorCallback);

		if (glfwInit() == GLFW_FALSE)
		{
			LOG_ERROR("Failed to initialize glfw.");
			return nullptr;
		}
		// glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		uint32 width = pCreateInfo->width;
		uint32 height = pCreateInfo->height;

		if (pCreateInfo->mode == WindowMode::Fullscreen)
		{
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
			auto monitor = glfwGetPrimaryMonitor();
			auto videoMode = glfwGetVideoMode(monitor);
			width = videoMode->width;
			height = videoMode->height;
		}
		else if (pCreateInfo->mode == WindowMode::Minimized)
		{
			glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		}

		if (pCreateInfo->resizable == false)
		{
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		}

		GLFWwindow* pGLFWwindow = glfwCreateWindow(width, height, pCreateInfo->title.c_str(), nullptr, nullptr);
		if (pGLFWwindow == nullptr)
		{
			LOG_ERROR("Failed to create glfw window.");
			return nullptr;
		}

		auto pWindow = new Window(width, height, pCreateInfo->mode);

		pWindow->mGLFWwindow = pGLFWwindow;

		pWindow->UpdateWindowSize_Internal();

		glfwSetWindowUserPointer(pGLFWwindow, pWindow);

		// Set event callbacks
		glfwSetWindowCloseCallback(pGLFWwindow, [](GLFWwindow* pGLFWwindow)
		{
			static Window* pWindow = (Window*)glfwGetWindowUserPointer(pGLFWwindow);
			if (!pWindow->mEventCallback) return;
			WindowCloseEvent event;
			pWindow->mEventCallback(event);
		});

		glfwSetWindowSizeCallback(pGLFWwindow, [](GLFWwindow* pGLFWwindow, int width, int height)
		{
			static Window* pWindow = (Window*)glfwGetWindowUserPointer(pGLFWwindow);
			pWindow->Resize(width, height); // Window resize callback is handled in here.
		});

		/**
		 * -----------------------X
		 * |
		 * |
		 * |
		 * Y
		 */
		glfwSetCursorPosCallback(pGLFWwindow, [](GLFWwindow* pGLFWwindow, double x, double y)
		{
			static Window* pWindow = (Window*)glfwGetWindowUserPointer(pGLFWwindow);
			if (!pWindow->mEventCallback) return;
			Vector2 screenPos = Vector2((float)x, (float)y);
			MouseMoveEvent event(screenPos * pWindow->mMousePosRatio, screenPos);
			pWindow->mEventCallback(event);
		});

		glfwSetMouseButtonCallback(pGLFWwindow, [](GLFWwindow* pGLFWwindow, int button, int action, int modifiers)
		{
			static Window* pWindow = (Window*)glfwGetWindowUserPointer(pGLFWwindow);
			if (!pWindow->mEventCallback) return;
			switch (action)
			{
				case GLFW_PRESS:
				{	
					double x, y;
					glfwGetCursorPos(pGLFWwindow, &x, &y);
					Vector2 screenPos = Vector2((float)x, (float)y);
					MouseButtonPressEvent event((MouseButtonID)button, screenPos * pWindow->mMousePosRatio, screenPos);
					pWindow->mEventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					double x, y;
					glfwGetCursorPos(pGLFWwindow, &x, &y);
					Vector2 screenPos = Vector2((float)x, (float)y);
					MouseButtonReleaseEvent event((MouseButtonID)button, screenPos * pWindow->mMousePosRatio, screenPos);
					pWindow->mEventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(pGLFWwindow, [](GLFWwindow* pGLFWwindow, double offsetX, double offsetY)
		{
			static Window* pWindow = (Window*)glfwGetWindowUserPointer(pGLFWwindow);
			if (!pWindow->mEventCallback) return;
			double x, y;
			glfwGetCursorPos(pGLFWwindow, &x, &y);
			Vector2 screenPos = Vector2((float)x, (float)y);
			MouseScrollEvent event(Vector2((float)offsetX, (float)offsetY), screenPos * pWindow->mMousePosRatio, screenPos);
			pWindow->mEventCallback(event);
		});

		glfwSetKeyCallback(pGLFWwindow, [](GLFWwindow* pGLFWwindow, int key, int scancode, int action, int modifiers)
		{
			static Window* pWindow = (Window*)glfwGetWindowUserPointer(pGLFWwindow);
			if (!pWindow->mEventCallback) return;
			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressEvent event((KeyCode)key, 0);
					pWindow->mEventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleaseEvent event((KeyCode)key);
					pWindow->mEventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressEvent event((KeyCode)key, 1);
					pWindow->mEventCallback(event);
					break;
				}
			}
		});

		if (pCreateInfo->mode == WindowMode::Minimized)
		{
			glfwIconifyWindow(pGLFWwindow);
			glfwShowWindow(pGLFWwindow);

		}
		else
		{
			glfwShowWindow(pGLFWwindow);
			glfwFocusWindow(pGLFWwindow);
		}

		return pWindow;
	}

	void Window::Resize(uint32 width, uint32 height)
	{
		if (width == 0 || height == 0)
		{
			mMode = WindowMode::Minimized;
		}
		else
		{
			mMode = WindowMode::Normal;
		}

		glfwSetWindowSize(mGLFWwindow, width, height);

		UpdateWindowSize_Internal();

		if(mEventCallback)
		{
			WindowResizeEvent event(mWidth, mHeight);
			mEventCallback(event);
		}
	}

	void Window::SetWindowPos(int32 x, int32 y)
	{
		glfwSetWindowPos(mGLFWwindow, x, y);
	}

	void Window::SetWindowTitle(const String& title)
	{
		glfwSetWindowTitle(mGLFWwindow, title.c_str());
	}

	void Window::PollEvents()
	{
		glfwPollEvents();
	}

	void Window::UpdateWindowSize_Internal()
	{
		int32 width, height;
		glfwGetWindowSize(mGLFWwindow, &width, &height);
		SetWindowSize_Internal(width, height);
	}

	void Window::SetWindowSize_Internal(uint32 width, uint32 height)
	{
		mWidth = width;
		mHeight = height;
		mMousePosRatio.x = 1.0f / (float)width;
		mMousePosRatio.y = 1.0f / (float)height;
	}
}
