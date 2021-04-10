#pragma once

#include "ComputeContext.h"

namespace Horizon
{
	class RenderContext : public ComputeContext
	{
	public:
		RenderContext(CommandQueue* queue);
		~RenderContext() = default;
		void BeginRenderPass(RenderPass* renderPass, Framebuffer* framebuffer, VkClearValue* pClearValues, uint32 clearValueCount);
		void EndRenderPass();
		void Draw(GraphicsState* state, uint32 vertexCount, uint32 firstVertex);
		void DrawInstancing(GraphicsState* state, uint32 vertexCount, uint32 firstVertex, uint32 instanceCount, uint32 firstInstance);
		void DrawIndexed(GraphicsState* state, uint32 indexCount, uint32 firstIndex, int32 vertexOffset);
		void DrawIndexedInstancing(GraphicsState* state, uint32 indexCount, uint32 firstIndex, int32 vertexOffset, uint32 instanceCount, uint32 firstInstance);
		void DrawIndirect();
		void DrawIndexedIndirect();
		void BindDescriptorSets(PipelineLayout* pipelineLayout, uint32 firstSet, DescriptorSet** descriptorSets, uint32 descriptorSetCount);
		void PushConstants(GraphicsState* state, VkShaderStageFlags shaderStageFlags, uint32 offset, uint32 size, const void* pValues);
	private:
		bool PrepareForDraw_Internal(GraphicsState* state);
	};
}
