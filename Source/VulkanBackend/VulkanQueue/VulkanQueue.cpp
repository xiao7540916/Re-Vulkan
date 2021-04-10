#include "VulkanQueue.h"
#include "VulkanDevice.h"
// #include "VulkanCommandBuffer.h"

namespace Horizon
{
	CommandQueue::CommandQueue(uint32 familyIndex, uint32 queueIndex)
		: mHandle(VK_NULL_HANDLE)
		, mFamilyIndex(familyIndex)
		, mQueueIndex(queueIndex)
		, mLastSubmittedCommandBuffer(nullptr)
		, mSubmitCounter(0)
	{
		vkGetDeviceQueue(gDevice->GetHandle(), familyIndex, queueIndex, &mHandle);
		assert(mHandle != VK_NULL_HANDLE);
	}

	CommandQueue::~CommandQueue()
	{
		assert(gDevice != nullptr);
	}

	void CommandQueue::WaitIdle()
	{
		// vkQueueWaitIdle is equivalent to submitting a fence to a queue and waiting with an infinite timeout for that fence to signal.
		vkQueueWaitIdle(mHandle);
	}

	void CommandQueue::SubmitCommandBuffer(CommandBuffer* cb)
	{
		// TODO
	}

	void CommandQueue::UpdateLastSubmittedCommandBuffer_Internal(CommandBuffer* cb)
	{
		mLastSubmittedCommandBuffer = cb;
	}
}
