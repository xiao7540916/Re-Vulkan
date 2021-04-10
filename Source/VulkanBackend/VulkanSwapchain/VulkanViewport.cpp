#include "VulkanViewport.h"
#include "VulkanSwapchain.h"
#include "VulkanDevice.h"

#include "Horizon/Log/LogManager.h"

namespace Horizon
{
	Viewport::Viewport(Device* device, void* window, bool isFullscreen)
		: mDevice(device)
		, mWindow(window)
		, mIsFullscreen(isFullscreen)
		, mSwapchain(nullptr)
		, mSwapchainImageCount(0)
		, mPresentCount(0)
		, mLockToVsync(false)
		, mPixelFormat(VK_FORMAT_B8G8R8A8_UNORM)
		, mCurrentSwapchainImageIndex((uint32)-1)
		, mBackBufferAvailableSemephore(VK_NULL_HANDLE)
	{
		SwapchainCreateInfo swapchainInfo;
		swapchainInfo.imageFormat = mPixelFormat;
		swapchainInfo.imageCount = kMaxSwapchainImageCount;
		swapchainInfo.lockToVsync = mLockToVsync;
		swapchainInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		mSwapchain = new Swapchain(mDevice, window, &swapchainInfo);

		mSwapchainImageCount = mSwapchain->GetImageCount();

		// Create semaphores.
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		for (uint32 backBufferIndex = 0; backBufferIndex < mSwapchainImageCount; backBufferIndex++)
		{
			VK_CHECK(vkCreateSemaphore(mDevice->GetHandle(), &semaphoreInfo, VULKAN_ALLOCATION_CALLBACKS, &mRenderCompleteSemaphores[backBufferIndex]));
		}
	}

	Viewport::~Viewport()
	{
		for (uint32 i = 0; i < mSwapchainImageCount; i++)
		{
			vkDestroySemaphore(mDevice->GetHandle(), mRenderCompleteSemaphores[i], VULKAN_ALLOCATION_CALLBACKS);
		}
		delete mSwapchain;
	}

	inline uint32 Viewport::GetWidth() const
	{
		return mSwapchain->GetWidth();
	}

	inline uint32 Viewport::GetHeight() const
	{
		return mSwapchain->GetHeight();
	}

	inline VkFormat Viewport::GetSwapchainImageFormat() const
	{
		return mSwapchain->GetImageFormat();
	}

	bool Viewport::PrepareForNextBackBuffer()
	{
		// Acquire next swapchain image.
		auto swapchainState = mSwapchain->AcquireNextImage(&mCurrentSwapchainImageIndex, &mBackBufferAvailableSemephore);
		if (swapchainState == Swapchain::State::OutOfDate || swapchainState == Swapchain::State::SurfaceLost)
		{
			RecreateSwapchain(mWindow);
			return false;
		}
		assert(mCurrentSwapchainImageIndex != (uint32)-1);
		return true;
	}

	bool Viewport::Present(CommandQueue* queue)
	{
		auto swapchainState = mSwapchain->Present(queue, &mRenderCompleteSemaphores[mCurrentSwapchainImageIndex]);
		if (swapchainState == Swapchain::State::OutOfDate || swapchainState == Swapchain::State::SubOptimal)
		{
			RecreateSwapchain(mWindow);
		}
		else if (swapchainState != Swapchain::State::Optimal)
		{
			return false;
		}
		mPresentCount++;
		return true;
	}

	void Viewport::Resize(uint32 width, uint32 height, bool isFullscreen)
	{
		RecreateSwapchain(mWindow);
	}

	void Viewport::RecreateSwapchain(void* window)
	{
		mDevice->WaitIdle();

		delete mSwapchain;
		SwapchainCreateInfo swapchainInfo;
		swapchainInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		//swapchainInfo.recreateInfo.surface = pSwapchain->GetSurfaceHandle();
		//swapchainInfo.recreateInfo.oldSwapchain = pSwapchain->GetHandle();
		mSwapchain = new Swapchain(mDevice, window, &swapchainInfo);
		assert(mSwapchain != nullptr);
		assert(swapchainInfo.recreateInfo.oldSwapchain == VK_NULL_HANDLE && swapchainInfo.recreateInfo.surface == VK_NULL_HANDLE);
		mSwapchainImageCount = mSwapchain->GetImageCount();
		if (mSwapchainImageCount > kMaxSwapchainImageCount)
		{
			LOG_ERROR("Horizon dose not support swapchain with {} images.", mSwapchainImageCount);
		}
	}
}
