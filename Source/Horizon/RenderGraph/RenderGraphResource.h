#pragma once

#include "Horizon/RenderBackend/RenderBackendCommon.h"

#include "RenderGraphHandle.h"
#include "RenderDependencyGraph.h"

namespace Horizon
{
	class ResourceNode;
	class RenderGraphPassExecutor;

	class RenderGraphVirtualResource
	{
	public:
		RenderGraphVirtualResource(RenderGraphHandleBase handle, ResourceNode* node, RenderGraphVirtualResource* parent = nullptr);
		virtual ~RenderGraphVirtualResource() = default;
		RenderGraphVirtualResource* GetParentResource();
		bool IsSubResource() const { return mParent != nullptr; }
		virtual bool IsImported() const { return false; }
		virtual void Devirtualize(RenderDevice* device) = 0;
		virtual void Destroy(RenderDevice* device) = 0;
	private:
		/// Parent resource. If this resource is not a subresource, parent must be nullptr.
		RenderGraphVirtualResource* mParent = nullptr;
		/// Resource node.
		ResourceNode* mNode = nullptr;
		/// Resource handle.
		RenderGraphHandleBase mHandle = RENDER_GRAPH_NULL_HANDLE;
	};

	class ResourceEdge : public RenderDependencyGraph::Edge
	{
	public:
		ResourceEdge(RenderGraph* rg, RenderDependencyGraph::Node* from, RenderDependencyGraph::Node* to, ResourceBindFlags flags);
		ResourceBindFlags GetResourceBindFlags() const { return mResourceBindFlags; }
	private:
		ResourceBindFlags mResourceBindFlags;
	};
	
	template<typename ResourceType>
	class RenderGraphResource : public RenderGraphVirtualResource
	{
	public:
		using Description = ResourceType::Description;
		using SubresourceDescription = ResourceType::SubresourceDescription;
		Resource(RenderGraphHandle<ResourceType> handle, ResouceNode* node, const Description& desc) 
			: RenderGraphVirtualResource(handle, node, nullptr), mResourceDescription(desc) {}
		Resource(RenderGraphHandle<ResourceType> handle, ResouceNode* node, const SubresourceDescription& desc, RenderGraphVirtualResource* parent) 
			: RenderGraphVirtualResource(handle, node, parent), mSubresourceDescription(desc) {}
		~RenderGraphResource() = default;
		RenderGraphHandle<ResourceType> GetHandle() const { return mHandle; }
		const ResourceType& Get() const { return mResource; }
		const ResourceDescription& GetDescription() const { return mResourceDescription; }
		const SubresourceDescription& GetSubresourceDescription() const { return mSubresourceDescription; }
		ResourceBindFlags GetBindFlags() const { return mBindFlags; }
	private:
		void Devirtualize(RenderDevice* device);
		void Destroy(RenderDevice* device);
		void ResolveBindFlags();
		/// Valid only after Devirtualize() has been called.
		ResourceType mResource = {};
		/// Valid only after ResolveBindFlags() has been called.
		ResourceBindFlags mBindFlags = ResourceBindFlags::None;
		Description mResourceDescription;
		SubresourceDescription mSubresourceDescription;
	};

	template<typename ResourceType>
	class RenderGraphImportedResource : public RenderGraphResource<ResourceType>
	{
		// TODO
	};
}
