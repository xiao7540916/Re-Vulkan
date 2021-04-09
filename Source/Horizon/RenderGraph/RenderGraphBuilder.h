#pragma once

#include "RenderGraphHandle.h"
#include "RenderGraphResource.h"

namespace Horizon
{
	class PassNode;
	class RenderGraph;

	class RenderGraphBuilder
	{
	public:
		template<typename ResourceType>
		RenderGraphHandle<ResourceType> Create(const char* name, const ResourceType::Description& desc);
		template<typename ResourceType>
		RenderGraphHandle<ResourceType> Read(RenderGraphHandle<ResourceType> input, ResourceBindFlags flags);
		template<typename ResourceType>
		RenderGraphHandle<ResourceType> Write(RenderGraphHandle<ResourceType> output, ResourceBindFlags flags);
		void AsynCompute();
		void SideEffect();
	private:
		friend class RenderGraph;
		RenderGraphBuilder(RenderGraph* rg, PassNode* pass) : mRenderGraph(rg), mPassNode(pass) {}
		~RenderGraphBuilder() = default;
		RenderGraph* mRenderGraph;
		PassNode* const mPassNode;
		bool mAsynCompute = false;
	};

	template<typename ResourceType>
	inline RenderGraphHandle<ResourceType> RenderGraphBuilder::Create(const char* name, const ResourceType::Description& desc)
	{
		return RenderGraphHandle<T>();
	}

	template<typename ResourceType>
	inline RenderGraphHandle<ResourceType> RenderGraphBuilder::Read(RenderGraphHandle<ResourceType> input, ResourceBindFlags flags)
	{
		VirtualResource* resource = mRenderGraph->GetResource(input);
		ResourceNode* node = GetActiveResourceNode();

		if (mRenderGraph->Connect(node, resource))
		{
			if (resource->isSubResource())
			{
				// this is read() to a subresource, so we need to add a "read" from the the parent's
				// node to the subresource -- but we may have two parent nodes, one for reads and
				// one for writes, so we need to use the one for reads.
				auto* parentNode = node->getParentNode();
				ResourceSlot& slot = getResourceSlot(parentNode->resourceHandle);
				if (slot.sid >= 0) {
					// we have a parent's node for reads, use the one
					parentNode = mResourceNodes[slot.sid];
				}
				node->setParentReadDependency(parentNode);
			}

			// if a resource has a subresource, then its handle becomes valid again as soon as it's used.
			ResourceSlot& slot = getResourceSlot(handle);
			if (slot.sid >= 0) {
				// we can now forget the "read" parent node, which becomes the current one again
				// until the next write.
				slot.sid = -1;
			}

			return handle;
		}

		return mRenderGraph.Read<T>(mPassNode, input, usage);
	}

	template<typename ResourceType>
	inline RenderGraphHandle<ResourceType> RenderGraphBuilder::Write(RenderGraphHandle<ResourceType> output, ResourceBindFlags flags)
	{
		return RenderGraphHandle<ResourceType>();
	}
}
