#include "VulkanInstance.h"
#include "Core/LogManager.h"

using namespace Horizon;

int main()
{
	LogManager::Init();

	InstanceInfo instanceInfo;
	instanceInfo.appName = "Vulkan Instance";
	instanceInfo.appVersion = 0;
	instanceInfo.AddInstanceLayer("VK_LAYER_KHRONOS_validation");
	instanceInfo.AddOptionalInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	instanceInfo.AddInstanceExtension(VK_KHR_SURFACE_EXTENSION_NAME);
	instanceInfo.AddInstanceExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#ifdef _WIN32
	instanceInfo.AddInstanceExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
	instanceInfo.AddInstanceExtension(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
	instanceInfo.AddInstanceExtension(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif

	Instance instance;
	if (!instance.Init(instanceInfo))
	{
		printf("Failed to init instance.");
		return EXIT_FAILURE;
	}
	instance.Destroy();
	return EXIT_SUCCESS;
}
