#include "Horizon/Core/Application.h"

namespace Horizon
{
	Application* Singleton<Application>::Instance = nullptr;

	Application::Application(const ApplicationCreateInfo& info)
		: mName(info.appName)
		, mVersion(info.appVersion)
		, mTimer()
		, mMainWindow(nullptr)
		, mShouldShutdown(false)
	{
		mMainWindow = Window::Create(&info.windowInfo);
		mMainWindow->SetEventCallback(HORIZON_BIND_FUNCTION(Application::EventCallback));
	}

	Application::~Application()
	{
		delete mMainWindow;
	}

	void Application::Run()
	{
		static float lastTime = 0.0f;
		mTimer.Reset();

		while (true)
		{
			mMainWindow->PollEvents();

			if (mShouldShutdown)
			{
				break;
			}

			if (mMainWindow->GetMode() == WindowMode::Minimized)
			{
				continue;
			}

			float currentTime = mTimer.ElapsedSeconds();
			Update(currentTime - lastTime);
			lastTime = currentTime;

			Render();
		}
	}

	void Application::EventCallback(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(HORIZON_BIND_FUNCTION(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(HORIZON_BIND_FUNCTION(Application::OnWindowResize));
		if (!e.IsHandled())
		{
			OnEvent(e);
		}
	}

	bool Application::OnWindowClose(const WindowCloseEvent& e)
	{
		SetShouldShutdown();
		return true;
	}

	void Application::SetShouldShutdown()
	{
		mShouldShutdown = true;
	}

	bool Application::OnWindowResize(const WindowResizeEvent& e)
	{
		if (mMainWindow->GetMode() == WindowMode::Minimized)
		{
			return true;
		}
		return false;
	}
}