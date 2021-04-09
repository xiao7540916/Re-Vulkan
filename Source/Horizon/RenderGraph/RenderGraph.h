#pragma once

#include "PassNode.h"
#include "ResourceNode.h"
#include "RenderDependencyGraph.h"
#include "RenderGraphPassExecutor.h"
#include "RenderGraphResource.h"
#include "RenderGraphHandle.h"
#include "RenderGraphRegistry.h"

namespace Horizon
{	
	class RenderDevice;
	
	class RenderGraph
	{
	public:
		RenderGraph() = default;
		~RenderGraph() = default;
		void Compile();
		void Execute(RenderDevice* device);
		template<typename Setup>
		void AddPass(const char* name, Setup setup);
		template<typename ResourceType>
		RenderGraphHandle<ResourceType> AddResource(char const* name, const ResourceType::Description& desc);
		template<typename ResourceType>
		RenderGraphHandle<ResourceType> AddSubresouce(char const* name, RenderGraphHandle<ResourceType> parent, const ResourceType::SubResourceDescription& desc);
		RenderDependencyGraph* GetGraph() { return &mGraph; }
		PassNode* GetPassNode(uint64 index) { return &mPassNodes[index]; }
		ResourceNode* GetResouceNode(uint64 index) { return &mResourceNodes[index]; }
	private:
		friend class RenderGraphRegistry;
		RenderGraphVirtualResource* GetResourceByHandle(RenderGraphHandleBase handle);
		RenderDependencyGraph mGraph;
		Vector<PassNode> mPassNodes;
		Vector<PassNode>::iterator mActivePassNodeBack;
		Vector<ResourceNode> mResourceNodes;
		Vector<RenderGraphPassExecutor> mPassExecutors;
		Vector<RenderGraphVirtualResource> mResources;
	};

	template<typename Setup>
	inline void RenderGraph::AddPass(const char* name, Setup setup)
	{
		mPassNodes.emplace_back(PassNode(this, name));
		PassNode* node = &mPassNodes.back();
		RenderGraphBuilder builder(this, node);
		mPasses.emplace_back(RenderGraphPass(node, setup(builder)));
		auto* pass = mPasses.back();
		node->SetPass(pass);
	}
	template<typename ResourceType>
	inline RenderGraphHandle<ResourceType> RenderGraph::AddResource(char const* name, const ResourceType::Description& desc)
	{
		mVirtualResources.emplace_back(RenderGraphResource<ResourceType>(name, desc));
		return RenderGraphHandle<ResourceType>();
	}

	template<typename ResourceType>
	inline RenderGraphHandle<ResourceType> RenderGraph::AddSubresouce(char const* name, RenderGraphHandle<ResourceType> parent, const ResourceType::SubResourceDescription& desc)
	{
		auto* parentResource = static_cast<RenderGraphResource<ResourceType>*>(GetResource(parent));
		mVirtualResources.emplace_back(RenderGraphResource<ResourceType>(name, parentResource, desc));
		return RenderGraphHandle<ResourceType>();
	}
}
