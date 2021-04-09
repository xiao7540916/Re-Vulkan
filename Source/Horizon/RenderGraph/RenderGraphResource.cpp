#include "RenderGraphResource.h"
#include "ResourceNode.h"
#include "RenderGraph.h"

namespace Horizon
{
	ResourceEdge::ResourceEdge(RenderGraph* rg, RenderDependencyGraph::Node* from, RenderDependencyGraph::Node* to, ResourceBindFlags flags)
		: RenderDependencyGraph::Edge(rg->GetGraph(), from, to)
		, mResourceBindFlags(flags)
	{
		
	}

	RenderGraphVirtualResource::RenderGraphVirtualResource(RenderGraphHandleBase handle, ResourceNode* node, RenderGraphVirtualResource* parent)
		: mHandle(handle)
		, mParent(parent)
		, mNode(node)
	{
		mNode->BindResource(this);
	}

	RenderGraphVirtualResource* RenderGraphVirtualResource::GetParentResource()
	{
		RenderGraphVirtualResource* p = this;
		while (p->mParent != nullptr) 
		{
			p = p->mParent;
		}
		return p;
	}
}
