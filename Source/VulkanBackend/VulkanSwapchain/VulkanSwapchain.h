#pragma once

#include "VulkanCommon.h"

namespace Horizon
{
	struct SwapchainCreateInfo
	{
		VkFormat            imageFormat = VK_FORMAT_B8G8R8A8_UNORM;    ///< Swapchain image format.
		bool                lockToVsync = false;                       ///< Enable / disable the vertical-sync.
		VkPresentModeKHR    presentMode = VK_PRESENT_MODE_FIFO_KHR;    ///< Present mode.
		uint32              imageCount = 3;                            ///< Number of swapchain images required to be created. In actual creation, it may be larger than this number.
		struct RecreateInfo
		{
			VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE;
			VkSurfaceKHR   surface = VK_NULL_HANDLE;
		};
		RecreateInfo recreateInfo = {};                                ///< Information needed to recreate the swapchain.
	};

	class Swapchain
	{
	public:
		enum class State
		{
			Optimal,
			SubOptimal,
			OutOfDate,
			SurfaceLost,
		};

		Swapchain(Device* device, void* windowHandle, SwapchainCreateInfo* createInfo);
		~Swapchain();

		void Destroy();

		inline VkSwapchainKHR GetHandle() const { return mHandle; }
		inline VkSurfaceKHR GetSurfaceHandle() const { return mSurface; }
		inline uint32 GetWidth() const { return mWidth; }
		inline uint32 GetHeight() const { return mHeight; }
		inline uint32 GetCurrentImageIndex() const { return mCurrentImageIndex; }
		inline VkFormat GetImageFormat() const { return mImageFormat; }
		inline uint32 GetImageCount() const { return (uint32)mImages.size(); }
		inline const Vector<VkImage>& GetImages() const { return mImages; }

		State Present(CommandQueue* presentQueue, VkSemaphore* renderCompleteSemaphore);
		State AcquireNextImage(uint32* outImageIndex, VkSemaphore* outSemaphore, Fence* outFence);

	private:

		Device*                mDevice;
		VkSwapchainKHR         mHandle;
		VkSurfaceKHR           mSurface;
		uint32                 mWidth;
		uint32                 mHeight;
		VkFormat               mImageFormat;
		Vector<VkImage>        mImages;
		uint32                 mPresentCount;
		uint32                 mSemaphoreIndex;
		uint32                 mCurrentImageIndex;
		Vector<Fence*>         mImageAcquiredFences;
		Vector<VkSemaphore>    mImageAcquiredSemaphores;
	};
}
