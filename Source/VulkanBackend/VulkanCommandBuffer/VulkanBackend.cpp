#include "VulkanBackend.h"
#include "VulkanDevice.h"

#include "Horizon/Log/LogManager.h"

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

	VulkanBackend::VulkanBackend(const VulkanBackendCreateInfo& info)
		: mInstance(VK_NULL_HANDLE)
		, mDebugUtilsMessenger(VK_NULL_HANDLE)
		, mDebugReportCallback(VK_NULL_HANDLE)
	{
		VulkanBridge bridge = {};
		if (info.enableValidationLayers)
		{
			bridge.instanceLayers.push_back("VK_LAYER_KHRONOS_validation");
			bridge.instanceLayers.push_back("VK_LAYER_LUNARG_monitor");
			bridge.optionalInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		bridge.instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		bridge.instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#ifdef _WIN32
		bridge.instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
		bridge.instanceExtensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
		bridge.instanceExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
		CreateInstance_Internal(info.applicationInfo, bridge);
	}

	VulkanBackend::~VulkanBackend()
	{
		for (auto* device : mDevices)
		{
			delete device;
		}
		assert(mInstance != VK_NULL_HANDLE);
		DestroyInstance_Internal();
	}

	bool VulkanBackend::CreateInstance_Internal(const ApplicationInfo& appInfo, const VulkanBridge& bridge)
	{
		LOG_TRACE("Initializing instance.");

		assert(!gInstanceInitialized);

		bool enableValidationLayers = bridge.instanceLayers.empty() ? false : true;
		if (enableValidationLayers)
		{
			uint32 supportedInstanceLayerCount = 0;
			VK_CHECK(vkEnumerateInstanceLayerProperties(&supportedInstanceLayerCount, nullptr));
			std::vector<VkLayerProperties> supportedInstanceLayers(supportedInstanceLayerCount);
			VK_CHECK(vkEnumerateInstanceLayerProperties(&supportedInstanceLayerCount, supportedInstanceLayers.data()));
			for (const auto& instanceLayer : supportedInstanceLayers)
			{
				LOG_TRACE("Supported instance layer: {} - vkspec version: {}.{}.{} {}: {}.",
					instanceLayer.layerName,
					VK_VERSION_MAJOR(instanceLayer.specVersion),
					VK_VERSION_MINOR(instanceLayer.specVersion),
					VK_VERSION_PATCH(instanceLayer.specVersion),
					instanceLayer.implementationVersion,
					instanceLayer.description);
			}

			const auto& requiredInstanceLayers = bridge.instanceLayers;
			for (const auto& requiredInstanceLayer : requiredInstanceLayers)
			{
				if (IsInstanceLayerSupported(requiredInstanceLayer, supportedInstanceLayers))
				{
					mEnabledInstanceLayers.emplace_back(requiredInstanceLayer);
					LOG_INFO("Enabled instance layer: {}.", requiredInstanceLayer);
				}
				else
				{
					LOG_ERROR("Missing required instance layer: {}.", requiredInstanceLayer);
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

		const auto& requiredInstanceExtensions = bridge.instanceExtensions;
		for (const auto& requiredInstancwExtension : requiredInstanceExtensions)
		{
			if (IsExtensionSupported(requiredInstancwExtension, supportedInstanceExtensions))
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

		const auto& optionalInstanceExtensions = bridge.optionalInstanceExtensions;
		for (const auto& optionalInstanceExtension : optionalInstanceExtensions)
		{
			if (IsExtensionSupported(optionalInstanceExtension, supportedInstanceExtensions))
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
				if (IsExtensionSupported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, supportedInstanceExtensions))
				{
					mEnabledInstanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
					LOG_INFO("Enabled instance extension: {}.", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
				}
				else if (IsExtensionSupported(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, supportedInstanceExtensions))
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

		VkApplicationInfo applicationInfo = {};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.apiVersion = VULKAN_API_VERSION;
		applicationInfo.pApplicationName = appInfo.pApplicationName;
		applicationInfo.applicationVersion = appInfo.applicationVersion;
		applicationInfo.pEngineName = appInfo.pEngineName;
		applicationInfo.engineVersion = appInfo.engineVersion;

		VkInstanceCreateInfo instanceInfo = {};
		instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pApplicationInfo = &applicationInfo;
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

		VK_CHECK(vkCreateInstance(&instanceInfo, VULKAN_ALLOCATION_CALLBACKS, &mInstance));

		uint32 physicalDeviceCount = 0;
		VK_CHECK(vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, nullptr));
		assert(physicalDeviceCount > 0 && "No available physical device.");
		mAvailablePhysicalDevices.resize(physicalDeviceCount);
		VK_CHECK(vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, mAvailablePhysicalDevices.data()));

		if (enableValidationLayers)
		{
			if (enableDebugUtils)
			{
				PFN_vkCreateDebugUtilsMessengerEXT createDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(mInstance, "vkCreateDebugUtilsMessengerEXT"));
				VK_CHECK(createDebugUtilsMessengerEXT(mInstance, &debugUtilsMessengerInfo, VULKAN_ALLOCATION_CALLBACKS, &mDebugUtilsMessenger));
			}
			else
			{
				PFN_vkCreateDebugReportCallbackEXT createDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(mInstance, "vkCreateDebugReportCallbackEXT"));
				VK_CHECK(createDebugReportCallbackEXT(mInstance, &debugReportCallbackInfo, VULKAN_ALLOCATION_CALLBACKS, &mDebugReportCallback));
			}
		}

		LOG_TRACE("Instance initialized.");

		gInstanceInitialized = true;

		return true;
	}

	void VulkanBackend::DestroyInstance_Internal()
	{
		LOG_TRACE("Destroy instance.");

		if (mDebugUtilsMessenger != VK_NULL_HANDLE)
		{
			PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(mInstance, "vkDestroyDebugUtilsMessengerEXT"));
			destroyDebugUtilsMessengerEXT(mInstance, mDebugUtilsMessenger, VULKAN_ALLOCATION_CALLBACKS);
			mDebugUtilsMessenger = VK_NULL_HANDLE;
		}
		if (mDebugReportCallback != VK_NULL_HANDLE)
		{
			PFN_vkDestroyDebugReportCallbackEXT destroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(mInstance, "vkDestroyDebugReportCallbackEXT"));
			destroyDebugReportCallbackEXT(mInstance, mDebugReportCallback, VULKAN_ALLOCATION_CALLBACKS);
			mDebugReportCallback = VK_NULL_HANDLE;
		}
		if (mInstance != VK_NULL_HANDLE)
		{
			vkDestroyInstance(mInstance, VULKAN_ALLOCATION_CALLBACKS);
			mInstance = VK_NULL_HANDLE;
		}
	}

	bool VulkanBackend::IsInstanceExtensionEnabled(const char* extension)
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

	RenderDevice* VulkanBackend::CreateRenderDevice(RenderDeviceExtensionFlags extensions)
	{
		VulkanDeviceExtensions deviceExtensions = {};
		VulkanPhysicalDeviceFeatures physicalDeviceFeatures = {};
		deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		deviceExtensions.push_back(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
		deviceExtensions.push_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
		if (extensions.bindlessResources)
		{
			deviceExtensions.push_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
			deviceExtensions.push_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
			VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT };
			VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR };
			physicalDeviceFeatures.Add(&descriptorIndexingFeatures);
			physicalDeviceFeatures.Add(&bufferDeviceAddressFeatures);
		}

		// Currently only supports one render device.
		Device* device = new Device(this, mInstance, mAvailablePhysicalDevices[0]);
		device->Init(deviceExtensions, physicalDeviceFeatures);
		mDevices.push_back(device);
		return device;
	}

	void VulkanBackend::DestroyRenderDevice(RenderDevice* device)
	{
		for (auto& it = mDevices.begin(); it != mDevices.end(); ++it)
		{
			if (device == *it)
			{
				((Device*)device)->Destroy();
				delete device;
				mDevices.erase(it);
				break;
			}
		}
	}
}