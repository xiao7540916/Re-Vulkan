#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanQueue.h"

#include "Horizon/Log/LogManager.h"

namespace Horizon
{
	CommandBuffer::CommandBuffer(Device* device, CommandBufferPool* pool)
		: mDevice(device)
		, mHandle(VK_NULL_HANDLE)
		, mCommandBufferPool(pool)
		, mState(State::UnInitialized)
	{
		assert(pool != nullptr);

		Init_Internal();

		mFence = mDevice->GetFenceManager().AllocateFence(true);
	}

	void CommandBuffer::Init_Internal()
	{
		VkCommandBufferAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.commandPool = mCommandBufferPool->GetHandle();
		// TODO: Secondary Command Buffer.
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandBufferCount = 1;
		VK_CHECK(vkAllocateCommandBuffers(mDevice->GetHandle(), &allocateInfo, &mHandle));

		mState = State::ReadyForBegin;
	}

	CommandBuffer::~CommandBuffer()
	{
		FenceManager& fenceManager = mDevice->GetFenceManager();
		if (mState == State::Submitted)
		{
			assert(fenceManager.IsFenceSignaled(mFence));
		}
		else
		{
			fenceManager.FreeFence(mFence);
		}
		if (mState != State::UnInitialized)
		{
			mCommandBufferPool->FreeCommandBuffer(this);
		}
	}

	void CommandBuffer::UpdateFenceStatus()
	{
		if (mState == State::Submitted)
		{
			FenceManager& fenceManager = mDevice->GetFenceManager();
			if (fenceManager.IsFenceSignaled(mFence))
			{
				++mFenceSignaledCounter;
				mState = State::ReadyForBegin;
			}
		}
	}

	void CommandBuffer::AddWaitSemaphore(VkPipelineStageFlags waitFlags, VkSemaphore waitSemaphore)
	{
		mWaitFlags.push_back(waitFlags);
		mWaitSemaphores.push_back(waitSemaphore);
	}

	void CommandBuffer::AddSignalSemaphore(VkSemaphore signalSemaphore)
	{
		mSignalSemaphores.push_back(signalSemaphore);
	}

	void CommandBuffer::Begin()
	{
		assert((mState == State::ReadyForBegin) && "Command buffer is not ready for recording.");

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VK_CHECK(vkBeginCommandBuffer(mHandle, &beginInfo));

		mState = State::Recording;
	}

	void CommandBuffer::End()
	{
		assert((mState == State::Recording) && "Command buffer is not recording, please call Begin() before End().");

		VK_CHECK(vkEndCommandBuffer(mHandle));

		mState = State::RecordingEnded;
	}

	void CommandBuffer::ResetFence_Internal()
	{
		mDevice->GetFenceManager().ResetFence(mFence);
	}

	void CommandBuffer::ClearWaitSemaphores_Internal()
	{
		mWaitFlags.clear();
		mWaitSemaphores.clear();
	}

	void CommandBuffer::ClearSignalSemaphores_Internal()
	{
		mSignalSemaphores.clear();
	}

	CommandBufferPool::CommandBufferPool(Device* device, CommandBufferManager& manager)
		: mDevice(device)
		, mHandle(VK_NULL_HANDLE)
		, mCommandBufferManager(manager)
	{

	}

	CommandBufferPool::~CommandBufferPool()
	{
		if (mHandle != VK_NULL_HANDLE)
		{
			for (uint64 i = 0; i < mActiveCommandBuffers.size(); i++)
			{
				if (mActiveCommandBuffers[i] != nullptr)
				{
					delete mActiveCommandBuffers[i];
				}
			}
			for (uint64 i = 0; i < mFreeCommandBuffers.size(); i++)
			{
				if (mFreeCommandBuffers[i] != nullptr)
				{
					delete mFreeCommandBuffers[i];
				}
			}
			vkDestroyCommandPool(mDevice->GetHandle(), mHandle, VULKAN_ALLOCATION_CALLBACKS);
			mHandle = VK_NULL_HANDLE;
		}
	}

	CommandBuffer* CommandBufferPool::AllocateCommandBuffer()
	{
		if (!mFreeCommandBuffers.empty())
		{
			CommandBuffer* cb = mFreeCommandBuffers.back();
			cb->Init_Internal();
			mFreeCommandBuffers.pop_back();
			mActiveCommandBuffers.push_back(cb);
			return cb;
		}
		CommandBuffer* cb = new CommandBuffer(mDevice, this);
		assert(cb != nullptr);
		mActiveCommandBuffers.push_back(cb);
		return cb;
	}

	void CommandBufferPool::FreeCommandBuffer(CommandBuffer* cb)
	{
		assert(cb->mHandle != VK_NULL_HANDLE);
		vkFreeCommandBuffers(mDevice->GetHandle(), mHandle, 1, &cb->GetHandle());
		cb->mHandle = VK_NULL_HANDLE;
		cb->mState = CommandBuffer::State::UnInitialized;
	}

	void CommandBufferPool::FreeUnusedCommandBuffers()
	{
		// TODO
	}

	void CommandBufferPool::Init_Internal(uint32 queueFamilyIndex)
	{
		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndex;
		VK_CHECK(vkCreateCommandPool(mDevice->GetHandle(), &poolInfo, VULKAN_ALLOCATION_CALLBACKS, &mHandle));
	}

	CommandBufferManager::CommandBufferManager(Device* device, QueueFamily family)
		: mDevice(device)
		, mQueueFamily(family)
		, mCommandPool(device, *this)
		, mCurrentCommandBuffer(nullptr)
	{
		mCommandPool.Init_Internal(device->GetQueueFamilyIndex(family));
		PrepareForNewCurrentCommandBuffer();
	}

	CommandBuffer* CommandBufferManager::PrepareForNewCurrentCommandBuffer()
	{
		for (CommandBuffer* activeCommandBuffer : mCommandPool.mActiveCommandBuffers)
		{
			activeCommandBuffer->UpdateFenceStatus();
			if (activeCommandBuffer->IsReadyForBegin())
			{
				mCurrentCommandBuffer = activeCommandBuffer;
				mCurrentCommandBuffer->Begin();
				return mCurrentCommandBuffer;
			}
			else
			{
				assert(activeCommandBuffer->IsSubmitted());
			}
		}
		mCurrentCommandBuffer = mCommandPool.AllocateCommandBuffer();
		mCurrentCommandBuffer->Begin();
		return mCurrentCommandBuffer;
	}

	void CommandBufferManager::SubmitCurrentCommandBuffer(CommandQueue* queue)
	{
		mCurrentCommandBuffer->End();
		queue->SubmitCommandBuffer(mCurrentCommandBuffer);
	}

	bool CommandBufferManager::WaitForCurrentCommandBuffer(uint64 timeInNanoseconds)
	{
		assert(mCurrentCommandBuffer->IsSubmitted());
		return mDevice->GetFenceManager().WaitForFence(mCurrentCommandBuffer->GetFence(), timeInNanoseconds);
	}

	void CommandBufferManager::FreeUnusedCommandBuffers()
	{
		mCommandPool.FreeUnusedCommandBuffers();
	}
}
