#include "VulkanInstance.h"
#include "Core/LogManager.h"

namespace Horizon
{
	bool gInstanceInitialized = false;

	VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
	{
		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			LOG_WARNING("{} - {}: {}", callbackData->messageIdNumber, callbackData->pMessageIdName, callbackData->pMessage);
		}
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			LOG_ERROR("{} - {}: {}", callbackData->messageIdNumber, callbackData->pMessageIdName, callbackData->pMessage);
		}
		return VK_FALSE;
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
		uint64_t object, size_t location, int32_t messageCode, const char* layerPrefix, const char* message, void* userData)
	{
		if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
		{
			LOG_ERROR("{}: {}", layerPrefix, message);
		}
		else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
		{
			LOG_WARNING("{}: {}", layerPrefix, message);
		}
		else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
		{
			LOG_WARNING("{}: {}", layerPrefix, message);
		}
		else
		{
			LOG_INFO("{}: {}", layerPrefix, message);
		}
		return VK_FALSE;
	}

	static bool IsInstanceLayerSupported(const char* name, const std::vector<VkLayerProperties>& supportedLayers)
	{
		for (const auto& supportedLayer : supportedLayers)
		{
			if (strcmp(name, supportedLayer.layerName) == 0)
			{
				return true;
			}
		}
		return false;
	}

	static bool IsInstanceExtensionSupported(const char* name, const std::vector<VkExtensionProperties>& supportedExtensions)
	{
		for (const auto& supportedExtension : supportedExtensions)
		{
			if (strcmp(name, supportedExtension.extensionName) == 0)
			{
				return true;
			}
		}
		return false;
	}

	Instance::Instance()
		: mHandle(VK_NULL_HANDLE)
		, mDebugUtilsMessenger(VK_NULL_HANDLE)
		, mDebugReportCallback(VK_NULL_HANDLE)
	{

	}

	Instance::~Instance()
	{
		if (mHandle != VK_NULL_HANDLE)
		{
			Destroy();
		}
	}

	bool Instance::Init(const InstanceInfo& info)
	{
		LOG_TRACE("Init instance.");

		assert(!gInstanceInitialized);

		bool enableValidationLayers = info.enableValidationLayers;
		if (enableValidationLayers)
		{
			uint32 validationLayerCount = 0;
			VK_CHECK(vkEnumerateInstanceLayerProperties(&validationLayerCount, nullptr));
			std::vector<VkLayerProperties> supportedValidationLayers(validationLayerCount);
			VK_CHECK(vkEnumerateInstanceLayerProperties(&validationLayerCount, supportedValidationLayers.data()));
			for (const auto& validationLayer : supportedValidationLayers)
			{
				LOG_TRACE("Supported instance layer: {} - vkspec version: {}.", validationLayer.layerName, validationLayer.specVersion);
			}

			const auto& requiredValidationLayers = info.instanceLayers;
			for (const auto& requiredValidationLayer : requiredValidationLayers)
			{
				if (IsInstanceLayerSupported(requiredValidationLayer, supportedValidationLayers))
				{
					mEnabledInstanceLayers.emplace_back(requiredValidationLayer);
					LOG_INFO("Enabled instance layer: {}.", requiredValidationLayer);
				}
				else
				{
					LOG_ERROR("Missing required instance layer: {}.", requiredValidationLayer);
					return false;
				}
			}
		}

		uint32 supportedInstanceExtensionCount = 0;
		VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &supportedInstanceExtensionCount, nullptr));
		std::vector<VkExtensionProperties> supportedInstanceExtensions(supportedInstanceExtensionCount);
		VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &supportedInstanceExtensionCount, supportedInstanceExtensions.data()));
		for (const auto& instanceExtension : supportedInstanceExtensions)
		{
			LOG_TRACE("Supported instance extension: {} - vkspec version: {}.", instanceExtension.extensionName, instanceExtension.specVersion);
		}

		const auto& requiredInstanceExtensions = info.instanceExtensions;
		for (const auto& requiredInstancwExtension : requiredInstanceExtensions)
		{
			if (IsInstanceExtensionSupported(requiredInstancwExtension, supportedInstanceExtensions))
			{
				mEnabledInstanceExtensions.emplace_back(requiredInstancwExtension);
				LOG_INFO("Enabled instance extension: {}.", requiredInstancwExtension);
			}
			else
			{
				LOG_ERROR("Missing required instance extension: {}.", requiredInstancwExtension);
				return false;
			}
		}

		const auto& optionalInstanceExtensions = info.optionalInstanceExtensions;
		for (const auto& optionalInstanceExtension : optionalInstanceExtensions)
		{
			if (IsInstanceExtensionSupported(optionalInstanceExtension, supportedInstanceExtensions))
			{
				mEnabledInstanceExtensions.emplace_back(optionalInstanceExtension);
				LOG_INFO("Enabled instance extension: {}.", optionalInstanceExtension);
			}
			else
			{
				LOG_WARNING("Missing optional instance extension: {}.", optionalInstanceExtension);
			}
		}

		bool enableDebugUtils = false;
		bool enableDebugReport = false;
		if (enableValidationLayers)
		{
			for (const auto& enabledInstanceExtension : mEnabledInstanceExtensions)
			{
				if (strcmp(enabledInstanceExtension, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
				{
					enableDebugUtils = true;
					break;
				}
				if (strcmp(enabledInstanceExtension, VK_EXT_DEBUG_REPORT_EXTENSION_NAME) == 0)
				{
					enableDebugReport = true;
				}
			}
			if ((!enableDebugUtils) && (!enableDebugReport))
			{
				LOG_WARNING("If validation layers are enabled. Extension: {} or {} must be enabled." VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
				if (IsInstanceExtensionSupported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, supportedInstanceExtensions))
				{
					mEnabledInstanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
					LOG_INFO("Enabled instance extension: {}.", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
				}
				else if (IsInstanceExtensionSupported(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, supportedInstanceExtensions))
				{
					mEnabledInstanceExtensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
					LOG_INFO("Enabled instance extension: {}.", VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
				}
				else
				{
					LOG_ERROR("Failed to enable instance layers.");
					return false;
				}
			}
		}

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.apiVersion = VULKAN_API_VERSION;
		appInfo.pApplicationName = info.appName;
		appInfo.applicationVersion = info.appVersion;
		appInfo.pEngineName = HORIZON_ENGINE_NAME;
		appInfo.engineVersion = HORIZON_ENGINE_VERSION;

		VkInstanceCreateInfo instanceInfo = {};
		instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pApplicationInfo = &appInfo;
		instanceInfo.enabledExtensionCount = (uint32)(mEnabledInstanceExtensions.size());
		instanceInfo.ppEnabledExtensionNames = mEnabledInstanceExtensions.data();
		instanceInfo.enabledLayerCount = 0;
		instanceInfo.ppEnabledLayerNames = nullptr;
		if (enableValidationLayers)
		{
			instanceInfo.enabledLayerCount = (uint32)(mEnabledInstanceLayers.size());
			instanceInfo.ppEnabledLayerNames = mEnabledInstanceLayers.data();
		}

		VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerInfo = {};
		VkDebugReportCallbackCreateInfoEXT debugReportCallbackInfo = {};
		if (enableValidationLayers)
		{
			debugUtilsMessengerInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
			debugReportCallbackInfo = { VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT };
			if (enableDebugUtils)
			{
				debugUtilsMessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
				debugUtilsMessengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
				debugUtilsMessengerInfo.pfnUserCallback = DebugUtilsMessengerCallback;

				instanceInfo.pNext = &debugUtilsMessengerInfo;
			}
			else
			{
				debugReportCallbackInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
				debugReportCallbackInfo.pfnCallback = DebugReportCallback;

				instanceInfo.pNext = &debugReportCallbackInfo;
			}
		}

		VK_CHECK(vkCreateInstance(&instanceInfo, VULKAN_ALLOCATION_CALLBACKS, &mHandle));

		if (enableValidationLayers)
		{
			if (enableDebugUtils)
			{
				PFN_vkCreateDebugUtilsMessengerEXT createDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(mHandle, "vkCreateDebugUtilsMessengerEXT"));
				VK_CHECK(createDebugUtilsMessengerEXT(mHandle, &debugUtilsMessengerInfo, VULKAN_ALLOCATION_CALLBACKS, &mDebugUtilsMessenger));
			}
			else
			{
				PFN_vkCreateDebugReportCallbackEXT createDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(mHandle, "vkCreateDebugReportCallbackEXT"));
				VK_CHECK(createDebugReportCallbackEXT(mHandle, &debugReportCallbackInfo, VULKAN_ALLOCATION_CALLBACKS, &mDebugReportCallback));
			}
		}

		LOG_TRACE("Instance initialized.");
		gInstanceInitialized = true;
		return true;
	}

	void Instance::Destroy()
	{
		LOG_TRACE("Destroy instance.");
		if (mDebugUtilsMessenger != VK_NULL_HANDLE)
		{
			PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(mHandle, "vkDestroyDebugUtilsMessengerEXT"));
			destroyDebugUtilsMessengerEXT(mHandle, mDebugUtilsMessenger, VULKAN_ALLOCATION_CALLBACKS);
			mDebugUtilsMessenger = VK_NULL_HANDLE;
		}
		if (mDebugReportCallback != VK_NULL_HANDLE)
		{
			PFN_vkDestroyDebugReportCallbackEXT destroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(mHandle, "vkDestroyDebugReportCallbackEXT"));
			destroyDebugReportCallbackEXT(mHandle, mDebugReportCallback, VULKAN_ALLOCATION_CALLBACKS);
			mDebugReportCallback = VK_NULL_HANDLE;
		}
		if (mHandle != VK_NULL_HANDLE)
		{
			vkDestroyInstance(mHandle, VULKAN_ALLOCATION_CALLBACKS);
			mHandle = VK_NULL_HANDLE;
		}
	}

	bool Instance::IsInstanceExtensionEnabled(const char* extension)
	{
		for (const auto& enabledExtension : mEnabledInstanceExtensions)
		{
			if (strcmp(extension, enabledExtension) == 0)
			{
				return true;
			}
		}
		return false;
	}
}