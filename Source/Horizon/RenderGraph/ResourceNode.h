#pragma once

#include "RenderGraphHandle.h"
#include "RenderDependencyGraph.h"

namespace Horizon
{
	class PassNode;
	class RenderGraph;
	class RenderGraphVirtualResource;

	class ResourceNode : public RenderDependencyGraph::Node
	{
	public:
		ResourceNode(RenderGraph* rg, const char* name, uint64 index, uint64 parentIndex);
		~ResourceNode();
		void AddReader(PassNode* passNode);
		void SetWriter(PassNode* passNode);
		ResourceNode* GetParentNode() const;
		auto* First() { return mFirst; }
		auto* Last() { return mLast; }
		bool HasAnyReader() const { return !mReaders.empty(); }
		bool HasWriter() const { return mWriter != nullptr; }
		uint64 GetIndex() const { return mIndex; }
		RenderGraphVirtualResource* GetResource() { return mResource; }
		const RenderGraphVirtualResource& GetResource() const { return *mResource; }
		void Graphvizify() const override;
	private:
		friend class RenderGraph;
		friend class RenderGraphVirtualResource;
		void BindResource(RenderGraphVirtualResource* resource) { mResource = resource; }
		RenderGraph* mRenderGraph;
		uint64 mIndex;
		uint64 mParentIndex;
		Vector<PassNode*> mReaders;
		PassNode* mWriter;
		/// Pass that needs to devirtualize this resource.
		PassNode* mFirst = nullptr;
		/// Pass that can destroy this resource.
		PassNode* mLast = nullptr;
		RenderGraphVirtualResource* mResource;
	};
}
