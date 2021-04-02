#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include "Core/LogManager.h"

using namespace Horizon;

int main()
{
	LogManager::Init();

	InstanceInitInfo instanceInfo;
	instanceInfo.applicationInfo.pApplicationName = "Vulkan Instance";
	instanceInfo.applicationInfo.applicationVersion = 0;
	instanceInfo.applicationInfo.pEngineName = HORIZON_ENGINE_NAME;
	instanceInfo.applicationInfo.engineVersion = HORIZON_ENGINE_VERSION;
	instanceInfo.enableValidationLayers = true;
	instanceInfo.instanceLayers.Add("VK_LAYER_KHRONOS_validation");
	instanceInfo.instanceLayers.Add("VK_LAYER_LUNARG_monitor");
	instanceInfo.optionalInstanceExtensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	instanceInfo.instanceExtensions.Add(VK_KHR_SURFACE_EXTENSION_NAME);
	instanceInfo.instanceExtensions.Add(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#ifdef _WIN32
	instanceInfo.instanceExtensions.Add(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
	instanceInfo.instanceExtensions.Add(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
	instanceInfo.instanceExtensions.Add(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif

	Instance instance;
	if (!instance.Init(instanceInfo))
	{
		LOG_ERROR("Failed to init instance.");
		return EXIT_FAILURE;
	}

	DeviceInitInfo deviceinfo;
	deviceinfo.deviceExtensions.Add(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	deviceinfo.deviceExtensions.Add(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
	deviceinfo.deviceExtensions.Add(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
	VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT };
	VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR };
	deviceinfo.physicalDeviceFeatures.Add(&descriptorIndexingFeatures);
	deviceinfo.physicalDeviceFeatures.Add(&bufferDeviceAddressFeatures);

	auto gpu = instance.GetAvailablePhysicalDevices()[0];

	Device device(&instance, gpu);
	device.Init(deviceinfo);

	// 如果你在定义了 VULKAN_ENABLE_DEBUG_MARKER 的情况下
	// 然后在 RenderDoc 里启动 Buid/Bin/VulkanDevice/VulkanDevice.exe
	// 你将会看到黄色的警告消失了 "Couldn't enable debug marker. Please note that debug marker can only be enabled for offline testing."
	// 说明 Debug Marker 被正常启用

	device.Destory();

	instance.Destroy();

	system("pause");

	return EXIT_SUCCESS;
}
