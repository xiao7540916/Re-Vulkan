#pragma once

#include "VulkanCommon.h"

namespace Horizon
{
	extern bool gInstanceInitialized;

	class InstanceLayers
	{
	public:
		void Add(const char* name)
		{
			mInstanceLayers.emplace_back(name);
		}
		const auto& GetData() const { return mInstanceLayers; }
	private:
		std::vector<const char*> mInstanceLayers;
	};

	class InstanceExtensions
	{
	public:
		void Add(const char* name)
		{
			mInstanceExtensions.emplace_back(name);
		}
		const auto& GetData() const { return mInstanceExtensions; }
	private:
		std::vector<const char*> mInstanceExtensions;
	};

	class OptionalInstanceExtensions
	{
	public:
		void Add(const char* name)
		{
			mOptionalInstanceExtensions.emplace_back(name);
		}
		const auto& GetData() const { return mOptionalInstanceExtensions; }
	private:
		std::vector<const char*> mOptionalInstanceExtensions;
	};

	struct InstanceInitInfo
	{
		struct ApplicationInfo
		{
			const char* pApplicationName;
			uint32 applicationVersion;
			const char* pEngineName;
			uint32 engineVersion;
		};
		ApplicationInfo applicationInfo;
		bool enableValidationLayers = VULKAN_DEFAULT_ENABLE_VALIDATION_LAYERS;
		InstanceLayers instanceLayers;
		InstanceExtensions instanceExtensions;
		OptionalInstanceExtensions optionalInstanceExtensions;
	};

	class Instance
	{
	public:

		Instance();
		~Instance();

		bool Init(const InstanceInitInfo& info);
		void Destroy();

		bool IsInstanceExtensionEnabled(const char* extension);

		inline VkInstance GetHandle() const { return mHandle; }
		inline const std::vector<const char*>& GetEnabledInstanceLayers() const { return mEnabledInstanceLayers; }
		inline const std::vector<const char*>& GetEnabledInstanceExtensions() const { return mEnabledInstanceExtensions; }
		inline const std::vector<VkPhysicalDevice>& GetAvailablePhysicalDevices() const { return mAvailablePhysicalDevices; }

	private:

		VkInstance mHandle;

		std::vector<const char*> mEnabledInstanceLayers;
		std::vector<const char*> mEnabledInstanceExtensions;

		VkDebugUtilsMessengerEXT mDebugUtilsMessenger;
		VkDebugReportCallbackEXT mDebugReportCallback;

		std::vector<VkPhysicalDevice> mAvailablePhysicalDevices;
	};
}
