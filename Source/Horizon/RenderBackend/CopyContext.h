#pragma once

// #include "Vulkan/CommandBuffer.h"

namespace Horizon
{
	class CopyContext
	{
	public:
		CopyContext(CommandQueue* queue);
		virtual ~CopyContext() = default;
		inline CommandQueue* GetCommandQueue() const { return mCommandQueue; }
		inline CommandBuffer* GetCurrentCommandBuffer() const { return mCurrentCommandBuffer; }
		inline CommandBufferManager* GetCommandBufferManager() { return &mCommandBufferManager; }
		void BeginRecording();
		void EndRecording();
		void SubmitCurrentCommandBuffer();
		void Flush();
		void CopyBufferToImage(Image* dstImage, const void* data, uint64 dataSize);
		void CopyImageToBuffer(Image* srcImage, Buffer* dstBuffer, VkOffset3D offset, VkExtent3D extent);
		void BufferMemoryBarrier(Buffer* buffer, uint64 offset, uint64 size, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);
		void ImageMemoryBarrier(Image* srcImage, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
		void CopyBuffer(const Buffer* srcBuffer, uint64 srcOffset, const Buffer* dstBuffer, uint64 dstOffset, uint32 byteCount);
		void CopyImage(const Image* srcImage, VkOffset3D srcOffset, VkImageLayout srcLayout, const VkImageSubresourceLayers& srcSubResource,
					   const Image* dstImage, VkOffset3D dstOffset, VkImageLayout dstLayout, const VkImageSubresourceLayers& dstSubResource, VkExtent3D extent);
	protected:
		CommandQueue* mCommandQueue;
		CommandBuffer* mCurrentCommandBuffer;
		CommandBufferManager mCommandBufferManager;
	};
}
