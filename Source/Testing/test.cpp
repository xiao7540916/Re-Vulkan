
#include <Horizon.h>

#include "Horizon/RenderBackend/Vulkan/VulkanBackend.h"

namespace Horizon
{
	class ExampleApplication : public Application
	{
	public:
		ExampleApplication(const ApplicationCreateInfo& info) : Application(info) {}
		void Init() override;
		void Shutdown() override;
		void OnEvent(Event& e) override;
		void Update(Timestep ts) override;
		void Render() override;
		bool OnKeyPress(const KeyPressEvent& e);
		bool OnWindowResize(const WindowResizeEvent& e);
	private:
		RenderBackend* mRenderBackend;
		RenderDevice* mRenderDevice;
		RenderViewportHandle mRenderViewport;
	};

	void ExampleApplication::Init()
	{
		VulkanBackendCreateInfo backendInfo;
		backendInfo.applicationInfo.pApplicationName = mName.c_str();
		backendInfo.applicationInfo.applicationVersion = mVersion;
		backendInfo.applicationInfo.pEngineName = HORIZON_ENGINE_NAME;
		backendInfo.applicationInfo.engineVersion = HORIZON_ENGINE_VERSION;
		backendInfo.enableValidationLayers = true;
		mRenderBackend = new VulkanBackend(backendInfo);
		RenderDeviceExtensionFlags extensionFlags = {};
		extensionFlags.bindlessResources = true;
		mRenderDevice = mRenderBackend->CreateRenderDevice(extensionFlags);
		mRenderViewport = mRenderDevice->CreateRenderViewport(GetMainWindow());
	}

	void ExampleApplication::Shutdown()
	{
		mRenderBackend->DestroyRenderDevice(mRenderDevice);
		delete mRenderBackend;
	}

	void ExampleApplication::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(HORIZON_BIND_FUNCTION(ExampleApplication::OnWindowResize));
		dispatcher.Dispatch<KeyPressEvent>(HORIZON_BIND_FUNCTION(ExampleApplication::OnKeyPress));
	}

	void ExampleApplication::Update(Timestep ts)
	{

	}

	void ExampleApplication::Render()
	{

	}

	bool ExampleApplication::OnKeyPress(const KeyPressEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case KeyCode::Esc:
			SetShouldShutdown();
			break;
		}
		return false;
	}

	bool ExampleApplication::OnWindowResize(const WindowResizeEvent& e)
	{
		uint32 width = e.GetWidth(); uint32 height = e.GetHeight();
		mRenderDevice->ResizeViewport(mRenderViewport, width, height);
		return false;
	}
}

namespace Horizon
{
	Application* CreateApplication(int argc, char** argv)
	{
		ApplicationCreateInfo appInfo;
		appInfo.appName = "Exampler Application";
		appInfo.appVersion = 0;
		appInfo.windowInfo.width = 1280;
		appInfo.windowInfo.height = 720;
		appInfo.windowInfo.title = "Hello Horizon!";
		try
		{
			auto app = new ExampleApplication(appInfo);
			return app;
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Failed to create application. Exception: {}.", e.what());
		}
		return nullptr;
	}
}