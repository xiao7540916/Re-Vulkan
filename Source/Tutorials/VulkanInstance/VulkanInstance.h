#pragma once

#include "VulkanCommon.h"

namespace Horizon
{
	extern bool gInstanceInitialized;

	struct InstanceInfo
	{
		const char* appName = nullptr;
		uint32 appVersion = 0;
		bool enableValidationLayers = VULKAN_DEFAULT_ENABLE_VALIDATION_LAYERS;
		void AddInstanceLayer(const char* name)
		{
			instanceLayers.emplace_back(name);
		}
		void AddInstanceExtension(const char* name)
		{
			instanceExtensions.emplace_back(name);
		}
		void AddOptionalInstanceExtension(const char* name)
		{
			optionalInstanceExtensions.emplace_back(name);
		}
		std::vector<const char*> instanceLayers;
		std::vector<const char*> instanceExtensions;
		std::vector<const char*> optionalInstanceExtensions;
	};

	class Instance
	{
	public:

		Instance();
		~Instance();

		bool Init(const InstanceInfo& info);
		void Destroy();

		bool IsInstanceExtensionEnabled(const char* extension);

	private:

		VkInstance mHandle;

		std::vector<const char*> mEnabledInstanceLayers;
		std::vector<const char*> mEnabledInstanceExtensions;

		VkDebugUtilsMessengerEXT mDebugUtilsMessenger;
		VkDebugReportCallbackEXT mDebugReportCallback;
	};
}
