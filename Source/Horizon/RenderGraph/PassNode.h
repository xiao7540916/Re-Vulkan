#pragma once

#include "RenderGraphRegistry.h"
#include "RenderDependencyGraph.h"

namespace Horizon
{
	class ResourceNode;
	class RenderGraph;
	class RenderGraphPassExecutor;

	class PassNode : public RenderDependencyGraph::Node
	{
	public:        
		using ResourceList = Vector<ResourceNode*>;
		PassNode(RenderGraph* rg, const char* name);
		~PassNode() = default;        
		const ResourceList& GetDevirtualizeList() const { return mDevirtualizeList; }
		const ResourceList& GetDestroyList() const { return mDestroyList; }
		RenderGraphPassExecutor* GetPassExecutor() { return mPassExecutor; }
		void GraphVizify() const override;
	protected:
		friend class RenderGraph;
		friend class RenderGraphPassExecutor;
		void BindPassExecutor(RenderGraphPassExecutor* passExecutor) { mPassExecutor = passExecutor; }
		RenderGraph* mRenderGraph;
		// Resources need to devirtualize before executing.
		ResourceList mDevirtualizeList;
		// Resources need to destroy after executing.
		ResourceList mDestroyList;
		RenderGraphPassExecutor* mPassExecutor = nullptr;
	};
}
