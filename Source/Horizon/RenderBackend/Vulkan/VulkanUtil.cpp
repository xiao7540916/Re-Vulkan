#include "VulkanUtil.h"

#include "Horizon/Log/LogManager.h"

namespace Horizon
{
	void HandleVkErrorCode(VkResult result, const char* vkFuntion, const char* filename, uint32 line)
	{
		if (result > 0)
		{
			LOG_ERROR("Unexpected result. Code: {}.Function : {}.File : {}.Line : {}.", (int)result, vkFuntion, filename, line);
		}
		else
		{
			LOG_ERROR("Vulkan function returns a runtime error. Code: {}. Function: {}. File: {}. Line: {}.", (int)result, vkFuntion, filename, line);
		}
	}

	bool IsExtensionSupported(const char* name, const std::vector<VkExtensionProperties>& supportedExtensions)
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
}