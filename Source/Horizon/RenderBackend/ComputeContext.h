#pragma once

#include "CopyContext.h"

namespace Horizon
{
	class ComputeContext : public CopyContext
	{
	public:
		static SharedPtr<ComputeContext> Create(CommandQueue* queue);
		ComputeContext(CommandQueue* queue);
		virtual ~ComputeContext() = default;
		virtual void BindPipeline(VkPipeline pipeline);
		void PushConstants(VkPipelineLayout layout, uint32 offset, uint32 size, const void* pValues);
		void BindDescriptorSets(VkPipelineLayout layout, uint32 firstSet, uint32 descriptorSetCount, const VkDescriptorSet* set);
		void Dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ);
		void DispatchIndirect(Buffer* buffer, uint64 offset);
		void Dispatch(ComputeState* pState, uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ);
	protected:
		Pipeline* mPrevBoundPipeline;
	private:
		bool IsPipelineAlreadyBound_Internal(ComputeState* pState);
		bool PrepareForDispatch_Internal(ComputeState* pState);
	};
}
