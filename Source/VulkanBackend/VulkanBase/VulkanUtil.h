#pragma once

#include <Horizon/Core/HorizonCommon.h>

#include <vulkan/vulkan.h>

namespace Horizon
{
	/**
	 * @brief This function determines how the application handles error code when a vulkan function failed.
	 * @remarks The application exits with an appropriate error message.
	 * @param { result } The VkResult enum.
	 * @param { vkFunction } The vulkan function which returns the result.
	 * @param { filename } The filename of the source file where the funtion was called.
	 * @param { line } The line number of the source file.
	 */
	extern void HandleVkErrorCode(VkResult result, const char* vkFuntion, const char* filename, uint32 line);

#ifndef VK_CHECK
	/**
	 * @brief Check that the vulkan funtion completed successfully. If it is not, handle the error code.
	 * @remarks All successful completion codes are non-negative values. All runtime error codes are negative values. (@see The Vulkan Sepcification)
	 */
#define VK_CHECK(VkFunction) { const VkResult result = VkFunction; if (result != VK_SUCCESS) { HandleVkErrorCode(result, #VkFunction, __FILE__, __LINE__); } }
#endif

#ifndef VK_CHECK_RESULT
#define VK_CHECK_RESULT(result) { if (result != VK_SUCCESS) { HandleVkErrorCode(result, __FUNCTION__, __FILE__, __LINE__); } }
#endif

	FORCEINLINE VkBool32 toVkBool(bool b)
	{
		return b ? VK_TRUE : VK_FALSE;
	}

	extern bool IsExtensionSupported(const char* name, const std::vector<VkExtensionProperties>& supportedExtensions);
}
