#pragma once

#include "Horizon/Core/HorizonCommon.h"
#include "Horizon/Core/Singleton.h"
#include "Horizon/Core/Timestep.h"
#include "Horizon/Core/Window.h"
#include "Horizon/Core/Timer.h"
#include "Horizon/Events/Events.h"

namespace Horizon
{
	struct ApplicationCreateInfo
	{
		String appName = "Horizon";
		uint32 appVersion = 0;
		WindowCreateInfo windowInfo;
	};

	class Application : public Singleton<Application>
	{
	public:
		Application(const ApplicationCreateInfo& info);
		virtual ~Application();
		virtual void Init() {}
		virtual void Run();
		virtual void Shutdown() {}
		virtual void OnEvent(Event& e) = 0;
		virtual void Update(Timestep ts) = 0;
		virtual void Render() = 0;
		void SetShouldShutdown();
		void EventCallback(Event& e);
		bool OnWindowClose(const WindowCloseEvent& e);
		bool OnWindowResize(const WindowResizeEvent& e);
		Window* GetMainWindow() { return mMainWindow; }
		const Window& GetMainWindow() const { return *mMainWindow; }
		const String& GetName() const { return mName; }
		const Timer& GetTimer() const { return mTimer; }
	protected:
		String mName;
		Timer mTimer;
		uint32 mVersion;
		Window* mMainWindow;
		bool mShouldShutdown;
	};

	/** This function should be implemented by CLIENT. */ 
	extern Application* CreateApplication(int argc, char** argv);
}
