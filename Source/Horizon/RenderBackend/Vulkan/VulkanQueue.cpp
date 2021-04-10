#include "VulkanQueue.h"
#include "VulkanDevice.h"
#include "VulkanFence.h"
#include "VulkanCommandBuffer.h"

namespace Horizon
{
	CommandQueue::CommandQueue(Device* device, uint32 familyIndex, uint32 queueIndex)
		: mDevice(device)
		, mHandle(VK_NULL_HANDLE)
		, mFamilyIndex(familyIndex)
		, mQueueIndex(queueIndex)
		, mLastSubmittedCommandBuffer(nullptr)
		, mSubmitCounter(0)
	{
		vkGetDeviceQueue(mDevice->GetHandle(), familyIndex, queueIndex, &mHandle);
		assert(mHandle != VK_NULL_HANDLE);
	}

	CommandQueue::~CommandQueue()
	{
		assert(mDevice->GetHandle() != VK_NULL_HANDLE);
	}

	void CommandQueue::WaitIdle()
	{
		// vkQueueWaitIdle is equivalent to submitting a fence to a queue and waiting with an infinite timeout for that fence to signal.
		vkQueueWaitIdle(mHandle);
	}

	void CommandQueue::SubmitCommandBuffer(CommandBuffer* cb)
	{
		assert(cb->mState == CommandBuffer::State::RecordingEnded);
		assert(!cb->mFence->IsSignaled());

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cb->mHandle;

		if (!cb->mSignalSemaphores.empty())
		{
			submitInfo.signalSemaphoreCount = (uint32)cb->mSignalSemaphores.size();
			submitInfo.pSignalSemaphores = cb->mSignalSemaphores.data();
		}

		if (!cb->mWaitSemaphores.empty())
		{
			assert(cb->mWaitSemaphores.size() == cb->mWaitFlags.size());

			submitInfo.waitSemaphoreCount = (uint32)cb->mWaitSemaphores.size();
			submitInfo.pWaitSemaphores = cb->mWaitSemaphores.data();
			submitInfo.pWaitDstStageMask = cb->mWaitFlags.data();
		}

		VK_CHECK(vkQueueSubmit(mHandle, 1, &submitInfo, cb->GetFence()->GetHandle()));

		cb->mState = CommandBuffer::State::Submitted;

		++cb->mSubmittedCounter;
		++mSubmitCounter;

		cb->ClearWaitSemaphores_Internal();
		cb->ClearSignalSemaphores_Internal();

		mLastSubmittedCommandBuffer = cb;
	}
}
