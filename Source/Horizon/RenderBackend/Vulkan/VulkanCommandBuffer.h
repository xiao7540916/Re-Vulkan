#pragma once

#include "VulkanCommon.h"

namespace Horizon
{
	class CommandBuffer
	{
	public:

		enum class State
		{
			UnInitialized,
			ReadyForBegin,
			Recording,
			InsideRenderPass,
			RecordingEnded,
			Submitted,
		};
		State mState;

		inline VkCommandBuffer& GetHandle() { return mHandle; }
		inline VkCommandBuffer GetHandle() const { return mHandle; }
		inline Fence* GetFence() { return mFence; }
		inline CommandBufferPool* GetAllocator() const { return mCommandBufferPool; }
		inline uint64 GetFenceSignaledCounter() const { return mFenceSignaledCounter; }

		void UpdateFenceStatus();
		void AddWaitSemaphore(VkPipelineStageFlags waitFlags, VkSemaphore waitSemaphore);
		void AddSignalSemaphore(VkSemaphore signalSemaphore);

		void Begin();
		void End();

		inline bool IsReadyForBegin() const { return mState == State::ReadyForBegin; }
		inline bool HasBegun() const { return mState == State::Recording || mState == State::InsideRenderPass; }
		inline bool HasEnded() const { return mState == State::RecordingEnded; }
		inline bool IsSubmitted() const { return mState == State::Submitted; }

	private:
		friend class CommandQueue;
		friend class CommandBufferPool;
		friend class CommandBufferManager;

		CommandBuffer(Device* device, CommandBufferPool* pool);
		~CommandBuffer();

		void Init_Internal();
		void ResetFence_Internal();
		void ClearWaitSemaphores_Internal();
		void ClearSignalSemaphores_Internal();

		Device* mDevice;
		VkCommandBuffer mHandle;
		CommandBufferPool* mCommandBufferPool;
		Fence* mFence;
		uint64 mSubmittedCounter;
		uint64 mFenceSignaledCounter;

		std::vector<VkSemaphore> mSignalSemaphores;
		std::vector<VkPipelineStageFlags> mWaitFlags;
		std::vector<VkSemaphore> mWaitSemaphores;
	};

	class CommandBufferPool
	{
	public:
		~CommandBufferPool();
		inline VkCommandPool GetHandle() const { return mHandle; }
		inline CommandBufferManager& GetCreator() { return mCommandBufferManager; }
		CommandBuffer* AllocateCommandBuffer();
		void FreeCommandBuffer(CommandBuffer* cb);
		void FreeUnusedCommandBuffers();
	private:
		friend class CommandBufferManager;
		CommandBufferPool(Device* mDevice, CommandBufferManager& manager);
		void Init_Internal(uint32 queueFamilyIndex);
		Device* mDevice;
		VkCommandPool mHandle;
		std::vector<CommandBuffer*> mActiveCommandBuffers;
		std::vector<CommandBuffer*> mFreeCommandBuffers;
		CommandBufferManager& mCommandBufferManager;
	};

	class CommandBufferManager
	{
	public:
		CommandBufferManager(Device* device, QueueFamily family);
		~CommandBufferManager() = default;
		inline VkCommandPool GetCommandPoolHandle() const { return mCommandPool.GetHandle(); }
		inline CommandBuffer* GetCurrentCommandBuffer() const { return mCurrentCommandBuffer; }
		CommandBuffer* PrepareForNewCurrentCommandBuffer();
		void SubmitCurrentCommandBuffer(CommandQueue* queue);
		bool WaitForCurrentCommandBuffer(uint64 timeInNanoseconds = UINT64_MAX);
		void FreeUnusedCommandBuffers();
	private:
		Device* mDevice;
		QueueFamily mQueueFamily;
		CommandBufferPool mCommandPool;
		CommandBuffer* mCurrentCommandBuffer;
	};
}
