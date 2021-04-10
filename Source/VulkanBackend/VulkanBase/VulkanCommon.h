#pragma once

#include "Horizon/Core/HorizonCommon.h"

#include <vulkan/vulkan.h>

#include "VulkanPlatform.h"
#include "VulkanConfig.h"
#include "VulkanUtil.h"

namespace Horizon
{
	enum class QueueFamily
	{
		Graphics = 0,
		Compute = 1,
		Transfer = 2,
		Count
	};

	/// Predefine classes
	class VulkanBackend;
	class Device;
	class Swapchain;
	class Viewport;
	class CommandQueue;
	class CommandBuffer;
	class Fence;
	class FenceManager;
	class CommandBufferPool;
}
