#include "RenderGraph.h"

#include "Horizon/RenderBackend/RenderDevice.h"
#include "Horizon/RenderBackend/RenderContext.h"
#include "Horizon/RenderBackend/ComputeContext.h"

namespace Horizon
{
	void RenderGraph::GraphVisfy() const
	{
		mGraph.GraphVizify();
	}

	RenderGraphVirtualResource* RenderGraph::GetResourceByHandle(RenderGraphHandleBase handle)
	{
		return &mResources[handle];
	}

	void RenderGraph::Compile()
	{
		mGraph.Cull();

		assert(mResourceNodes.size() == mResources.size());
		assert(mPassNodes.size() == mPassExecutors.size());

		mActivePassNodeBack = std::stable_partition(mPassNodes.begin(), mPassNodes.end(), [](auto const& passNode)
		{
			return !passNode.IsCulled();
		});

		auto first = mPassNodes.begin();
		while(first != mActivePassNodeBack)
		{
			PassNode& passNode = *first;
        	first++;

			const auto& reads = passNode.GetIncomingEdges();
			for (auto* edge : reads)
			{
				ResourceNode* resourceNode = static_cast<ResourceNode*>(edge->from);
				passNode->RegisterResource(resourceNode->GetResourceHandle());
			}

			const auto& writes = passNode.GetOutgoingEdges();
			for (auto* edge : writes)
			{
				// A output node might be culled.
				if (!edge->IsValid()) continue;
				ResourceNode* resourceNode = static_cast<ResourceNode*>(edge->to);
				passNode->RegisterResource(resourceNode->GetResourceHandle());
			}

			passNode->Resolve();
		}

		for (ResourceNode& resourceNode : mResourceNodes) 
		{
			if (resourceNode.IsCulled()) continue;

			RenderGraphVirtualResource* resource = resourceNode.GetResource();
			PassNode* first = resource->first;
			PassNode* last = resource->last;
			assert(!first == !last);
			if (first && last) 
			{
				assert(!first->IsCulled());
				assert(!last->IsCulled());
				first->mDevirtualizeList.push_back(resource);
				last->mDestroyList.push_back(resource);
			}
		}
	}

	void RenderGraph::Execute(RenderDevice* device)
	{
		auto* renderContext = device->GetRenderContext();
		device->BeginDebugMarker(renderContext->GetCurrentCommandBuffer(), "RenderGraph");

		auto first = mPassNodes.begin();
		while(first != mActivePassNodeBack)
		{
			PassNode& passNode = *first;
			RenderGraphPassExecutor* pass = passNode.GetPassExecutor();
        	first++;

			bool bAsynCompute = pass->IsAsynCompute();
			void* context = bAsynCompute ? (void*)device->GetComputeContext() : (void*)device->GetRenderContext();

			device->BeginDebugMarker(context->GetCurrentCommandBuffer(), passNode.GetName());

			// Devirtualize resources.
			for (RenderGraphVirtualResource* resource : pass->GetDevirtualizeList())
			{
				assert(resource->First() == pass);
				resource->Devirtualize(device);
			}

			// Call execute.
			RenderGraphRegistry registry(this, &passNode);
			pass->Execute(registry, context);

			// Destroy resources.
			for (RenderGraphVirtualResource* resource : pass->GetDestroyList())
			{
				assert(resource->Last() == pass);
				resource->Destroy(device);
			}

			device->EndDebugMarker(context->GetCurrentCommandBuffer());
		}
		device->EndDebugMarker(renderContext->GetCurrentCommandBuffer());
		device->Flush();
	}
}
