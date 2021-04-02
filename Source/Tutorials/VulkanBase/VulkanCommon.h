#pragma once

#include <vulkan/vulkan.h>

#include "Core/HorizonCommon.h"

#include "VulkanPlatform.h"
#include "VulkanConfig.h"
#include "VulkanUtil.h"

namespace Horizon
{
	enum class QueueFamily
	{
		Graphics,
		Compute,
		Transfer,
		Count
	};

	/// Predefine classes
	class Instance;
	class CommandQueue;
}
