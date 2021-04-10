#pragma once

#include "VulkanCommon.h"

namespace Horizon
{
	class Viewport
	{
	public:

		Viewport(Device* device, void* window, bool isFullscreen = false);

		~Viewport();

		bool PrepareForNextBackBuffer();

		bool Present(CommandQueue* queue);

		void Resize(uint32 width, uint32 height, bool isFullscreen);

		inline bool IsFullscreen() const { return mIsFullscreen; }

		inline uint32 GetWidth() const;

		inline uint32 GetHeight() const;

		inline VkFormat GetSwapchainImageFormat() const;

		inline Swapchain* GetSwapChain() { return mSwapchain; }

		inline uint32 GetPresentCount() const { return mPresentCount; }

	private:

		friend class VulkanBackend;

		/** Maxmum number of swapchain image. */
		static const uint32 kMaxSwapchainImageCount = 3;

		void RecreateSwapchain(void* window);

		Device*        mDevice;
		void*          mWindow;
		Swapchain*     mSwapchain;
		bool           mIsFullscreen;
		VkFormat       mPixelFormat;
		bool           mLockToVsync;
		uint32         mPresentCount;
		uint32         mSwapchainImageCount;
		uint32         mCurrentSwapchainImageIndex;
		VkSemaphore    mBackBufferAvailableSemephore;    ///< Get from swapchain.
		VkSemaphore    mRenderCompleteSemaphores[kMaxSwapchainImageCount];
	};
}
