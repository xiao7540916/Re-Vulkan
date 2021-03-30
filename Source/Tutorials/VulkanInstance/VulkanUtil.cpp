#include "VulkanUtil.h"
#include "Core/LogManager.h"

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
}