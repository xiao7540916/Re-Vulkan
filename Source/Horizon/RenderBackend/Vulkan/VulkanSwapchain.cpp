#include "VulkanSwapchain.h"
#include "VulkanDevice.h"
#include "VulkanQueue.h"

#include "Horizon/Core/Window.h"
#include "Horizon/Log/LogManager.h"

namespace Horizon
{
	static VkSurfaceKHR CreateSurface(VkInstance instance, VkPhysicalDevice physicalDevice, Window* pWindow, uint32 queueFamilyIndex)
	{
		VkSurfaceKHR surface;
		VK_CHECK(glfwCreateWindowSurface(instance, pWindow->GetGLFWwindow(), nullptr, &surface));

		VkBool32 supported = VK_FALSE;
		VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, &supported));
		assert(supported == VK_TRUE);

		return surface;
	}

	static uint32 ChooseImageCount(uint32 requestedImageCount, uint32 minImageCount, uint32 maxImageCount)
	{
		// 0 means no limit
		if (maxImageCount != 0)
		{
			requestedImageCount = std::min(requestedImageCount, maxImageCount);
		}
		requestedImageCount = std::max(requestedImageCount, minImageCount);
		return requestedImageCount;
	}

	static VkPresentModeKHR ChoosePresentMode(VkPresentModeKHR requestedPresentMode, const std::vector<VkPresentModeKHR>& availablePresentModes, bool lockToVsync)
	{
		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
		if (lockToVsync == false)
		{
			for (const auto& availablePresentMode : availablePresentModes)
			{
				if (requestedPresentMode == availablePresentMode)
				{
					return requestedPresentMode;
				}
			}
			for (const auto& availablePresentMode : availablePresentModes)
			{
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					return availablePresentMode;
				}
				else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
				{
					bestMode = availablePresentMode;
				}
			}
		}
		return bestMode;
	}

	static VkExtent2D ChooseExtent(VkExtent2D requestedExtent, const VkExtent2D& minImageExtent, const VkExtent2D& maxImageExtent, const VkExtent2D& currentExtent)
	{
		if (requestedExtent.width < 1 || requestedExtent.height < 1)
		{
			return currentExtent;
		}

		requestedExtent.width = std::max(requestedExtent.width, minImageExtent.width);
		requestedExtent.width = std::min(requestedExtent.width, maxImageExtent.width);

		requestedExtent.height = std::max(requestedExtent.height, minImageExtent.height);
		requestedExtent.height = std::min(requestedExtent.height, maxImageExtent.height);

		return requestedExtent;
	}

	static VkSurfaceFormatKHR ChooseSurfaceFormat(VkFormat requestedColorFormat, VkColorSpaceKHR requestedColorSpace, const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == requestedColorFormat && availableFormat.colorSpace == requestedColorSpace)
			{
				return availableFormat;
			}
		}
		if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
		{
			return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}
		return availableFormats[0];
	}

	Swapchain::Swapchain(Device* device, void* windowHandle, SwapchainCreateInfo* createInfo)
		: mDevice(device)
		, mHandle(VK_NULL_HANDLE)
		, mSurface(VK_NULL_HANDLE)
		, mWidth(0)
		, mHeight(0)
		, mImageFormat(VK_FORMAT_UNDEFINED)
		, mSemaphoreIndex(0)
		, mPresentCount(0)
		, mCurrentImageIndex((uint32)-1)
	{
		// Recreate
		bool recreate = false;
		auto& recreateInfo = createInfo->recreateInfo;
		if (recreateInfo.oldSwapchain != VK_NULL_HANDLE)
		{
			assert(recreateInfo.surface != VK_NULL_HANDLE);
			recreate = true;
		}

		Window* window = (Window*)windowHandle;
		// Surface
		if (recreate)
		{
			mSurface = recreateInfo.surface;
			recreateInfo.surface = VK_NULL_HANDLE;
		}
		else
		{
			mSurface = CreateSurface(mDevice->GetInstanceHandle(), mDevice->GetPhysicalDeviceHandle(), window, mDevice->GetGraphicsQueue()->GetFamilyIndex());
		}

		// Present mode
		uint32 presentModeCount = 0;
		VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->GetPhysicalDeviceHandle(), mSurface, &presentModeCount, nullptr));
		std::vector<VkPresentModeKHR> availablePresentModes(presentModeCount);
		VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->GetPhysicalDeviceHandle(), mSurface, &presentModeCount, availablePresentModes.data()));
		for (const auto& presentMode : availablePresentModes)
		{
			LOG_TRACE("Available present mode: {}.", (int)presentMode);
		}
		VkPresentModeKHR presentMode = ChoosePresentMode(createInfo->presentMode, availablePresentModes, createInfo->lockToVsync);
		if (presentMode != createInfo->presentMode)
		{
			LOG_WARNING("Requested present mode not found.");
		}

		// Image format
		uint32 surfaceFormatCount;
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->GetPhysicalDeviceHandle(), mSurface, &surfaceFormatCount, nullptr));
		assert(surfaceFormatCount != 0);
		std::vector<VkSurfaceFormatKHR> availableSurfaceFormats(surfaceFormatCount);
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->GetPhysicalDeviceHandle(), mSurface, &surfaceFormatCount, availableSurfaceFormats.data()));

		const VkFormat requestedFormat = createInfo->imageFormat;
		const VkColorSpaceKHR requestedColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

		VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(requestedFormat, requestedColorSpace, availableSurfaceFormats);
		mImageFormat = surfaceFormat.format;
		if (mImageFormat != createInfo->imageFormat)
		{
			LOG_WARNING("Requested image format not found.");
		}

		// Surface capabilities
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice->GetPhysicalDeviceHandle(), mSurface, &surfaceCapabilities));
		assert(surfaceCapabilities.supportedUsageFlags & (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT));
		VkSurfaceTransformFlagBitsKHR preTransform;
		if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		{
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		else
		{
			preTransform = surfaceCapabilities.currentTransform;
		}
		VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
		if (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
		{
			compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		}

		// Image Count
		uint32 imageCount = ChooseImageCount(createInfo->imageCount, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount);

		// Image extent
		VkExtent2D imageExtent = {};
		if (surfaceCapabilities.currentExtent.width == (uint32)-1)
		{
			imageExtent.width = window->GetWidth();
			imageExtent.height = window->GetHeight();
		}
		else
		{
			imageExtent = surfaceCapabilities.currentExtent;
		}
		imageExtent = ChooseExtent(imageExtent, surfaceCapabilities.minImageExtent, surfaceCapabilities.maxImageExtent, surfaceCapabilities.currentExtent);

		mWidth = imageExtent.width;
		mHeight = imageExtent.height;

		VkSwapchainCreateInfoKHR swapchainInfo = {};
		swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainInfo.surface = mSurface;
		swapchainInfo.presentMode = presentMode;
		swapchainInfo.minImageCount = imageCount;
		swapchainInfo.imageFormat = surfaceFormat.format;
		swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
		swapchainInfo.imageExtent.width = mWidth;
		swapchainInfo.imageExtent.height = mHeight;
		swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		swapchainInfo.imageArrayLayers = 1;
		swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainInfo.preTransform = preTransform;
		swapchainInfo.compositeAlpha = compositeAlpha;
		swapchainInfo.clipped = VK_TRUE;
		swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
		if (recreate)
		{
			swapchainInfo.oldSwapchain = recreateInfo.oldSwapchain;
		}

		LOG_INFO("Creating new vulkan swapchain with width: {}, height: {}, present mode: {}, format: {}, color space: {}, requested image count: {}.",
			mWidth, mHeight, (uint32)swapchainInfo.presentMode, (uint32)swapchainInfo.imageFormat, (uint32)swapchainInfo.imageColorSpace, (uint32)swapchainInfo.minImageCount);

		VK_CHECK(vkCreateSwapchainKHR(mDevice->GetHandle(), &swapchainInfo, VULKAN_ALLOCATION_CALLBACKS, &mHandle));

		if (recreate)
		{
			if (recreateInfo.oldSwapchain != VK_NULL_HANDLE)
			{
				vkDestroySwapchainKHR(mDevice->GetHandle(), recreateInfo.oldSwapchain, VULKAN_ALLOCATION_CALLBACKS);
				recreateInfo.oldSwapchain = VK_NULL_HANDLE;
			}
			if (recreateInfo.surface != VK_NULL_HANDLE)
			{
				vkDestroySurfaceKHR(mDevice->GetInstanceHandle(), recreateInfo.surface, VULKAN_ALLOCATION_CALLBACKS);
				recreateInfo.surface = VK_NULL_HANDLE;
			}
		}

		uint32 swapchianImageCount = 0;
		VK_CHECK(vkGetSwapchainImagesKHR(mDevice->GetHandle(), mHandle, &swapchianImageCount, nullptr));
		mImages.resize(swapchianImageCount);
		VK_CHECK(vkGetSwapchainImagesKHR(mDevice->GetHandle(), mHandle, &swapchianImageCount, mImages.data()));

		FenceManager& fenceManager = mDevice->GetFenceManager();
		if (recreate)
		{
			for (uint64 i = 0; i < mImageAcquiredFences.size(); i++)
			{
				fenceManager.FreeFence(mImageAcquiredFences[i]);
			}
			for (uint64 i = 0; i < mImageAcquiredSemaphores.size(); i++)
			{
				vkDestroySemaphore(mDevice->GetHandle(), mImageAcquiredSemaphores[i], VULKAN_ALLOCATION_CALLBACKS);
				mImageAcquiredSemaphores[i] = VK_NULL_HANDLE;
			}
		}

		mImageAcquiredFences.resize(swapchianImageCount);
		for (uint32 imageIndex = 0; imageIndex < swapchianImageCount; imageIndex++)
		{
			mImageAcquiredFences[imageIndex] = fenceManager.AllocateFence(true);
		}

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		mImageAcquiredSemaphores.resize(swapchianImageCount);
		for (uint32 imageIndex = 0; imageIndex < swapchianImageCount; imageIndex++)
		{
			assert(mImageAcquiredSemaphores[imageIndex] == VK_NULL_HANDLE);
			VK_CHECK(vkCreateSemaphore(mDevice->GetHandle(), &semaphoreInfo, VULKAN_ALLOCATION_CALLBACKS, &mImageAcquiredSemaphores[imageIndex]));
		}
	}

	Swapchain::~Swapchain()
	{
		if (mHandle != VK_NULL_HANDLE)
		{
			Destroy();
		}
	}

	void Swapchain::Destroy()
	{
		mDevice->WaitIdle();

		vkDestroySwapchainKHR(mDevice->GetHandle(), mHandle, VULKAN_ALLOCATION_CALLBACKS);
		mHandle = VK_NULL_HANDLE;

		vkDestroySurfaceKHR(mDevice->GetInstanceHandle(), mSurface, VULKAN_ALLOCATION_CALLBACKS);
		mSurface = VK_NULL_HANDLE;

		FenceManager& fenceManager = mDevice->GetFenceManager();
		for (uint64 i = 0; i < mImageAcquiredFences.size(); i++)
		{
			fenceManager.FreeFence(mImageAcquiredFences[i]);
		}

		for (uint64 i = 0; i < mImageAcquiredSemaphores.size(); i++)
		{
			vkDestroySemaphore(mDevice->GetHandle(), mImageAcquiredSemaphores[i], VULKAN_ALLOCATION_CALLBACKS);
			mImageAcquiredSemaphores[i] = VK_NULL_HANDLE;
		}
	}

	Swapchain::State Swapchain::AcquireNextImage(uint32* outImageIndex, VkSemaphore* outSemaphore)
	{
		assert(mCurrentImageIndex == (uint32)-1);

		const uint32 prevSemaphoreIndex = mSemaphoreIndex;
		mSemaphoreIndex = (mSemaphoreIndex + 1) % (uint32)mImageAcquiredSemaphores.size();

		FenceManager& fenceManager = mDevice->GetFenceManager();
		fenceManager.ResetFence(mImageAcquiredFences[mSemaphoreIndex]);

		const VkSemaphore imageAcquiredSemaphore = mImageAcquiredSemaphores[mSemaphoreIndex];
		const Fence* imageAcquiredFence = mImageAcquiredFences[mSemaphoreIndex];

		uint32 imageIndex = 0;
		VkResult result = vkAcquireNextImageKHR(mDevice->GetHandle(), mHandle, UINT64_MAX, imageAcquiredSemaphore, imageAcquiredFence->GetHandle(), &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			mSemaphoreIndex = prevSemaphoreIndex;
			return State::OutOfDate;
		}
		if (result == VK_ERROR_SURFACE_LOST_KHR)
		{
			mSemaphoreIndex = prevSemaphoreIndex;
			return State::SurfaceLost;
		}
		if (result == VK_SUBOPTIMAL_KHR)
		{
			return State::SubOptimal;
		}
		VK_CHECK_RESULT(result);

		mCurrentImageIndex = imageIndex;
		*outImageIndex = mCurrentImageIndex;
		*outSemaphore = mImageAcquiredSemaphores[mSemaphoreIndex];

		bool ret = fenceManager.WaitForFence(mImageAcquiredFences[mSemaphoreIndex], UINT64_MAX);
		assert(ret);

		return State::Optimal;
	}

	Swapchain::State Swapchain::Present(CommandQueue* presentQueue, VkSemaphore* renderCompleteSemaphore)
	{
		assert(mCurrentImageIndex != (uint32)-1);

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		if (renderCompleteSemaphore != nullptr)
		{
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = renderCompleteSemaphore;
		}
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &mHandle;
		presentInfo.pImageIndices = &mCurrentImageIndex;

		mPresentCount++;

		VkResult result = vkQueuePresentKHR(presentQueue->GetHandle(), &presentInfo);

		mCurrentImageIndex = (uint32)-1;

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			return State::OutOfDate;
		}
		if (result == VK_ERROR_SURFACE_LOST_KHR)
		{
			return State::SurfaceLost;
		}
		if (result == VK_SUBOPTIMAL_KHR)
		{
			return State::SubOptimal;
		}
		VK_CHECK_RESULT(result);

		return State::Optimal;
	}
}