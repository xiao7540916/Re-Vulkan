#pragma once

#include "Horizon/Core/HorizonCommon.h"
#include "Horizon/RenderBackend/RenderBackend.h"
#include "Horizon/RenderBackend/RenderHandles.h"

#include "VulkanCommon.h"

namespace Horizon
{
	extern bool gInstanceInitialized;

	struct VulkanBackendCreateInfo
	{
		ApplicationInfo applicationInfo = {};
		bool enableValidationLayers = VULKAN_DEFAULT_ENABLE_VALIDATION_LAYERS;
	};

	struct VulkanBridge
	{
		Vector<const char*> instanceLayers;
		Vector<const char*> instanceExtensions;
		Vector<const char*> optionalInstanceExtensions;
	};
	
	class VulkanBackend : public RenderBackend
	{
	public:

		VulkanBackend(const VulkanBackendCreateInfo& info);
		~VulkanBackend();

		RenderDevice* CreateRenderDevice(RenderDeviceExtensionFlags extensions) override final;
		void DestroyRenderDevice(RenderDevice* device) override final;

		bool IsInstanceExtensionEnabled(const char* extension);

		inline const Vector<const char*>& GetEnabledInstanceLayers() const { return mEnabledInstanceLayers; }
		inline const Vector<const char*>& GetEnabledInstanceExtensions() const { return mEnabledInstanceExtensions; }
		inline const Vector<VkPhysicalDevice>& GetAvailablePhysicalDevices() const { return mAvailablePhysicalDevices; }

	private:

		bool CreateInstance_Internal(const ApplicationInfo& appInfo, const VulkanBridge& bridge);
		void DestroyInstance_Internal();

		VkInstance mInstance;

		Vector<const char*> mEnabledInstanceLayers;
		Vector<const char*> mEnabledInstanceExtensions;

		bool mIsDebugUtilsExtensionSupported = false;
		bool mIsDebugReportExtensionSupported = false;
		VkDebugUtilsMessengerEXT mDebugUtilsMessenger;
		VkDebugReportCallbackEXT mDebugReportCallback;

		Vector<VkPhysicalDevice> mAvailablePhysicalDevices;

		/** Currently only supports one render device. */
		Vector<Device*> mDevices;
	};
}
