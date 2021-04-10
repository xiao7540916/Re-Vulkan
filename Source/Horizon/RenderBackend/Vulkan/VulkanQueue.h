#pragma once

#include "VulkanCommon.h"

namespace Horizon
{
	class CommandQueue
	{
	public:

		CommandQueue(Device* device, uint32 familyIndex, uint32 queueIndex);
		~CommandQueue();

		inline VkQueue GetHandle() const { return mHandle; }
		inline uint32 GetFamilyIndex() const { return mFamilyIndex; }
		inline uint32 GetQueueIndex() const { return mQueueIndex; }
		inline uint64 GetSubmitCount() const { return mSubmitCounter; }

		void WaitIdle();

		void SubmitCommandBuffer(CommandBuffer* cb);

	private:

		Device* mDevice;
		VkQueue mHandle;
		uint32 mFamilyIndex;
		uint32 mQueueIndex;
		uint64 mSubmitCounter;
		CommandBuffer* mLastSubmittedCommandBuffer;
	};
}
