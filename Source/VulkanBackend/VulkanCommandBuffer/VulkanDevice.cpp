#include "VulkanDevice.h"
#include "VulkanBackend.h"
#include "VulkanQueue.h"
#include "VulkanViewport.h"
#include "VulkanCommandBuffer.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include "Horizon/Log/LogManager.h"

namespace Horizon
{
#if defined(VULKAN_ENABLE_DEBUG_MARKER)
	void Device::SetupDebugMarker()
	{
		mDebugMarker.vkDebugMarkerSetObjectTag = reinterpret_cast<PFN_vkDebugMarkerSetObjectTagEXT>(vkGetDeviceProcAddr(mHandle, "vkDebugMarkerSetObjectTagEXT"));
		mDebugMarker.vkDebugMarkerSetObjectName = reinterpret_cast<PFN_vkDebugMarkerSetObjectNameEXT>(vkGetDeviceProcAddr(mHandle, "vkDebugMarkerSetObjectNameEXT"));
		mDebugMarker.vkCmdDebugMarkerBegin = reinterpret_cast<PFN_vkCmdDebugMarkerBeginEXT>(vkGetDeviceProcAddr(mHandle, "vkCmdDebugMarkerBeginEXT"));
		mDebugMarker.vkCmdDebugMarkerEnd = reinterpret_cast<PFN_vkCmdDebugMarkerEndEXT>(vkGetDeviceProcAddr(mHandle, "vkCmdDebugMarkerEndEXT"));
		mDebugMarker.vkCmdDebugMarkerInsert = reinterpret_cast<PFN_vkCmdDebugMarkerInsertEXT>(vkGetDeviceProcAddr(mHandle, "vkCmdDebugMarkerInsertEXT"));
		assert(mDebugMarker.vkDebugMarkerSetObjectTag != VK_NULL_HANDLE);
		assert(mDebugMarker.vkDebugMarkerSetObjectName != VK_NULL_HANDLE);
		assert(mDebugMarker.vkCmdDebugMarkerBegin != VK_NULL_HANDLE);
		assert(mDebugMarker.vkCmdDebugMarkerEnd != VK_NULL_HANDLE);
		assert(mDebugMarker.vkCmdDebugMarkerInsert != VK_NULL_HANDLE);
		mDebugMarker.device = mHandle;
		mDebugMarker.initialized = true;
	}

	void Device::DebugMarker::SetObjectName(uint64 object, VkDebugReportObjectTypeEXT objectType, const char* name)
	{
		if (initialized)
		{
			VkDebugMarkerObjectNameInfoEXT nameInfo = {};
			nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
			nameInfo.objectType = objectType;
			nameInfo.object = object;
			nameInfo.pObjectName = name;
			vkDebugMarkerSetObjectName(device, &nameInfo);
		}
	}

	void Device::DebugMarker::SetObjectTag(uint64 object, VkDebugReportObjectTypeEXT objectType, uint64 tagName, uint64 tagSize, const void* tag)
	{
		if (initialized)
		{
			VkDebugMarkerObjectTagInfoEXT tagInfo = {};
			tagInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_TAG_INFO_EXT;
			tagInfo.objectType = objectType;
			tagInfo.object = object;
			tagInfo.tagName = tagName;
			tagInfo.tagSize = tagSize;
			tagInfo.pTag = tag;
			vkDebugMarkerSetObjectTag(device, &tagInfo);
		}
	}

	void Device::DebugMarker::Begin(VkCommandBuffer cb, const char* pMarkerName, const float color[4])
	{
		if (initialized)
		{
			VkDebugMarkerMarkerInfoEXT markerInfo = {};
			markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
			memcpy(markerInfo.color, &color[0], sizeof(float) * 4);
			markerInfo.pMarkerName = pMarkerName;
			vkCmdDebugMarkerBegin(cb, &markerInfo);
		}
	}

	void Device::DebugMarker::Insert(VkCommandBuffer cb, const char* pMarkerName, const float color[4])
	{
		if (initialized)
		{
			VkDebugMarkerMarkerInfoEXT markerInfo = {};
			markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
			memcpy(markerInfo.color, &color[0], sizeof(float) * 4);
			markerInfo.pMarkerName = pMarkerName;
			vkCmdDebugMarkerInsert(cb, &markerInfo);
		}
	}

	void Device::DebugMarker::End(VkCommandBuffer cb)
	{
		if (initialized)
		{
			vkCmdDebugMarkerEnd(cb);
		}
	}
#endif // VULKAN_ENABLE_DEBUG_MARKER

	Device::Device(VulkanBackend* backend, VkInstance instance, VkPhysicalDevice gpu)
		: mBackend(backend)
		, mInstance(instance)
		, mHandle(VK_NULL_HANDLE)
		, mGpu(gpu)
		, mMemoryAllocator(VK_NULL_HANDLE)
		, mGpuProperties({})
		, mGpuFeatures({})
		, mFenceManager(this)
	{
		assert(instance != nullptr);
	}

	Device::~Device()
	{
		assert(mHandle == VK_NULL_HANDLE);
	}

	void Device::Init(const VulkanDeviceExtensions& extensions, const VulkanPhysicalDeviceFeatures& features)
	{
		InitPhysicalDevice_Internal();
		CreteaLogicalDevice_Internal(extensions, features);
		CreateMemoryAllocator_Internal();
		mFenceManager.Init();
	}


	void Device::Destroy()
	{
		assert(mHandle != VK_NULL_HANDLE);

		for (auto* viewport : mViewports)
		{
			delete viewport;
		}

		// Destroy fence manager.
		mFenceManager.Destroy();

		// Release queues
		for (uint32 family = 0; family < kQueueFamilyCount; family++)
		{
			for (uint64 queueIndex = 0; queueIndex < mCommandQueues[family].size(); queueIndex++)
			{
				if (mCommandQueues[family][queueIndex] != nullptr)
				{
					delete mCommandQueues[family][queueIndex];
				}
			}
		}

		// Destroy vma handle.
		if (mMemoryAllocator != VK_NULL_HANDLE)
		{
			VmaStats stats;
			vmaCalculateStats(mMemoryAllocator, &stats);
			LOG_INFO("Total device memory leaked: {} bytes.", (uint32)stats.total.usedBytes);
			vmaDestroyAllocator(mMemoryAllocator);
		}
		// Destroy handle.
		if (mHandle != VK_NULL_HANDLE)
		{
			vkDestroyDevice(mHandle, VULKAN_ALLOCATION_CALLBACKS);
			mHandle = VK_NULL_HANDLE;
		}
	}

	void Device::InitPhysicalDevice_Internal()
	{
		vkGetPhysicalDeviceProperties(mGpu, &mGpuProperties);

		// Check vulkan api version
		uint32 vulkanApiVersion = VK_MAKE_VERSION(VULKAN_API_MAJOR_VERSION, VULKAN_API_MINOR_VERSION, VULKAN_API_PATCH_VERSION);
		if (mGpuProperties.apiVersion < vulkanApiVersion)
		{
			String requestedVersion = std::to_string(VULKAN_API_MAJOR_VERSION) + "." + std::to_string(VULKAN_API_MINOR_VERSION);
			String supportedVersion = std::to_string(VK_VERSION_MAJOR(mGpuProperties.apiVersion)) + "." + std::to_string(VK_VERSION_MINOR(mGpuProperties.apiVersion));
			LOG_ERROR("Vulkan device does not support requested API version. Requested version: {}, Highest supported: {}.", requestedVersion, supportedVersion);
			return;
		}

		uint32 queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(mGpu, &queueFamilyCount, nullptr);
		mQueueFamilyProperties.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(mGpu, &queueFamilyCount, mQueueFamilyProperties.data());

		for (uint32 family = 0; family < HORIZON_ARRAYSIZE(mQueueFamilyIndices); family++)
		{
			mQueueFamilyIndices[family] = (uint32)-1;
		}

		uint32& graphicsQueueFamilyIndex = mQueueFamilyIndices[(uint32)QueueFamily::Graphics];
		uint32& computeQueueFamilyIndex = mQueueFamilyIndices[(uint32)QueueFamily::Compute];
		uint32& transferQueueFamilyIndex = mQueueFamilyIndices[(uint32)QueueFamily::Transfer];

		for (uint32 i = 0; i < (uint32)mQueueFamilyProperties.size(); i++)
		{
			VkQueueFlags flags = mQueueFamilyProperties[i].queueFlags;
			if ((flags & VK_QUEUE_GRAPHICS_BIT) != 0 && graphicsQueueFamilyIndex == (uint32)-1)
			{
				graphicsQueueFamilyIndex = i;
			}
			else if ((flags & VK_QUEUE_COMPUTE_BIT) != 0 && computeQueueFamilyIndex == (uint32)-1)
			{
				computeQueueFamilyIndex = i;
			}
			else if ((flags & VK_QUEUE_TRANSFER_BIT) != 0 && transferQueueFamilyIndex == (uint32)-1)
			{
				transferQueueFamilyIndex = i;
			}
			LOG_INFO("Queue family index : {}, queue flags :{}, queue count {}.", i, flags, mQueueFamilyProperties[i].queueCount);
		}

		mSeparateGraphicsComputeQueue = (graphicsQueueFamilyIndex == computeQueueFamilyIndex) ? false : true;

		LOG_INFO("Graphics queue family index is: {}.", graphicsQueueFamilyIndex);
		LOG_INFO("Compute queue family index is: {}.", computeQueueFamilyIndex);
		LOG_INFO("Transfer queue family index is: {}.", transferQueueFamilyIndex);
	}

	void Device::CreteaLogicalDevice_Internal(const VulkanDeviceExtensions& extensions, const VulkanPhysicalDeviceFeatures& features)
	{
		uint32 deviceExtensionCount = 0;
		VK_CHECK(vkEnumerateDeviceExtensionProperties(mGpu, nullptr, &deviceExtensionCount, nullptr));
		std::vector<VkExtensionProperties> supportedDeviceExtensions(deviceExtensionCount);
		VK_CHECK(vkEnumerateDeviceExtensionProperties(mGpu, nullptr, &deviceExtensionCount, supportedDeviceExtensions.data()));
		for (const auto& deviceExtension : supportedDeviceExtensions)
		{
			LOG_TRACE("Supported device extension: {} - vkspec version: {}.", deviceExtension.extensionName, deviceExtension.specVersion);
		}

#if defined(VULKAN_ENABLE_DEBUG_MARKER)
		if (IsExtensionSupported(VK_EXT_DEBUG_MARKER_EXTENSION_NAME, supportedDeviceExtensions))
		{
			mEnabledDeviceExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
			LOG_INFO("Enabled device extension: {}.", VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
		}
#endif // VULKAN_ENABLE_DEBUG_MARKER

		const auto& requiredDeviceExtensions = extensions;
		for (const auto& requiredExtension : requiredDeviceExtensions)
		{
			if (IsExtensionSupported(requiredExtension, supportedDeviceExtensions))
			{
				mEnabledDeviceExtensions.push_back(requiredExtension);
				LOG_INFO("Enabled device extension: {}.", requiredExtension);
			}
			else
			{
				LOG_ERROR("The device doesn't support the required extension: {}.", requiredExtension);
			}
		}

		std::vector<VkDeviceQueueCreateInfo> queueInfos = {};
		std::vector<std::vector<float>> queuePriorities(HORIZON_ARRAYSIZE(mQueueFamilyIndices));
		for (uint32 family = 0; family < HORIZON_ARRAYSIZE(mQueueFamilyIndices); family++)
		{
			const uint32 queueCount = mQueueCount[family];
			// Set all priorities to 1.0 for now.
			queuePriorities[family].resize(queueCount, 1.0f);
			mCommandQueues[family].resize(queueCount);

			VkDeviceQueueCreateInfo queueInfo = {};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueCount = queueCount;
			queueInfo.queueFamilyIndex = mQueueFamilyIndices[family];
			queueInfo.pQueuePriorities = queuePriorities[family].data();
			if (queueInfo.queueCount > 0)
			{
				queueInfos.push_back(queueInfo);
			}
		}

		mGpuFeatures = {};
		mGpuFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		mGpuFeatures.pNext = features.GetEntry();

		vkGetPhysicalDeviceFeatures2(mGpu, &mGpuFeatures);

		mEnabledValidationLayers = mBackend->GetEnabledInstanceLayers();

		VkDeviceCreateInfo deviceInfo = {};
		deviceInfo.pNext = &mGpuFeatures; // Vulkan 1.2
		deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceInfo.queueCreateInfoCount = (uint32)(queueInfos.size());
		deviceInfo.pQueueCreateInfos = queueInfos.data();
		deviceInfo.pEnabledFeatures = nullptr;
		deviceInfo.enabledExtensionCount = (uint32)(mEnabledDeviceExtensions.size());
		deviceInfo.ppEnabledExtensionNames = mEnabledDeviceExtensions.data();
		deviceInfo.enabledLayerCount = (uint32)(mEnabledValidationLayers.size());
		deviceInfo.ppEnabledLayerNames = mEnabledValidationLayers.data();

		VK_CHECK(vkCreateDevice(mGpu, &deviceInfo, VULKAN_ALLOCATION_CALLBACKS, &mHandle));

#if defined(VULKAN_ENABLE_DEBUG_MARKER)
		if (IsDeviceExtensionEnabled(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
		{
			SetupDebugMarker();
		}
		else
		{
			LOG_WARNING("Couldn't enable debug marker. Please note that debug marker can only be enabled for offline testing.");
		}
#endif // VULKAN_ENABLE_DEBUG_MARKER

		// Init command queues.
		for (uint32 family = 0; family < HORIZON_ARRAYSIZE(mQueueFamilyIndices); family++)
		{
			for (uint32 queueIndex = 0; queueIndex < (uint32)mCommandQueues[family].size(); queueIndex++)
			{
				LOG_INFO("Creating command queue, queue family index: {}, queue index {}.", mQueueFamilyIndices[family], queueIndex);
				mCommandQueues[family][queueIndex] = new CommandQueue(this, mQueueFamilyIndices[family], queueIndex);
			}
		}
	}

	bool Device::IsDeviceExtensionEnabled(const char* extension)
	{
		for (const auto& enabledExtension : mEnabledDeviceExtensions)
		{
			if (strcmp(extension, enabledExtension) == 0)
			{
				return true;
			}
		}
		return false;
	}

	RenderViewportHandle Device::CreateRenderViewport(void* window)
	{
		RenderViewportHandle handle = mViewports.size();
		mViewports.push_back(new Viewport(this, window));
		return handle;
	}

	void Device::ResizeViewport(RenderViewportHandle handle, uint32 width, uint32 height, bool isFullscreen)
	{
		mViewports[handle]->Resize(width, height, isFullscreen);
	}

	bool Device::WaitForCommandBuffer(CommandBuffer* cb, uint64 timeInNanoseconds)
	{
		return mFenceManager.WaitForFence(cb->GetFence(), timeInNanoseconds);
	}

	void Device::CreateMemoryAllocator_Internal()
	{
		VmaVulkanFunctions vmaVulkanFunctions = {};
		vmaVulkanFunctions.vkAllocateMemory = vkAllocateMemory;
		vmaVulkanFunctions.vkBindBufferMemory = vkBindBufferMemory;
		vmaVulkanFunctions.vkBindImageMemory = vkBindImageMemory;
		vmaVulkanFunctions.vkCreateBuffer = vkCreateBuffer;
		vmaVulkanFunctions.vkCreateImage = vkCreateImage;
		vmaVulkanFunctions.vkDestroyBuffer = vkDestroyBuffer;
		vmaVulkanFunctions.vkDestroyImage = vkDestroyImage;
		vmaVulkanFunctions.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
		vmaVulkanFunctions.vkFreeMemory = vkFreeMemory;
		vmaVulkanFunctions.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
		vmaVulkanFunctions.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
		vmaVulkanFunctions.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
		vmaVulkanFunctions.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
		vmaVulkanFunctions.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
		vmaVulkanFunctions.vkMapMemory = vkMapMemory;
		vmaVulkanFunctions.vkUnmapMemory = vkUnmapMemory;
		vmaVulkanFunctions.vkCmdCopyBuffer = vkCmdCopyBuffer;

		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = mGpu;
		allocatorInfo.device = mHandle;
		allocatorInfo.instance = mInstance;

		if (IsDeviceExtensionEnabled(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME) && IsDeviceExtensionEnabled(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME))
		{
			allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
			PFN_vkGetBufferMemoryRequirements2KHR getBufferMemoryRequirements2KHR = reinterpret_cast<PFN_vkGetBufferMemoryRequirements2KHR>(vkGetInstanceProcAddr(mInstance, "vkGetBufferMemoryRequirements2KHR"));
			PFN_vkGetImageMemoryRequirements2KHR getImageMemoryRequirements2KHR = reinterpret_cast<PFN_vkGetImageMemoryRequirements2KHR>(vkGetInstanceProcAddr(mInstance, "vkGetImageMemoryRequirements2KHR"));
			vmaVulkanFunctions.vkGetBufferMemoryRequirements2KHR = getBufferMemoryRequirements2KHR;
			vmaVulkanFunctions.vkGetImageMemoryRequirements2KHR = getImageMemoryRequirements2KHR;
		}
		else
		{
			LOG_WARNING("Missing device extensions: {} and {}.", VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME, VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
		}

		if (IsDeviceExtensionEnabled(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME))
		{
			allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		}

		allocatorInfo.pVulkanFunctions = &vmaVulkanFunctions;

		VK_CHECK(vmaCreateAllocator(&allocatorInfo, &mMemoryAllocator));
	}

	void Device::WaitIdle()
	{
		VK_CHECK(vkDeviceWaitIdle(mHandle));
	}
}