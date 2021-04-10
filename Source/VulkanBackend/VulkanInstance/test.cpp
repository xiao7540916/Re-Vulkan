#include "VulkanInstance.h"

#include <Horizon/Log/LogManager.h>

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

	VulkanContext instance;
	if (!instance.Init(instanceInfo))
	{
		LOG_ERROR("Failed to init instance.");
		return EXIT_FAILURE;
	}
	instance.Destroy();
	return EXIT_SUCCESS;
}
