#pragma once

#include "Horizon/RenderBackend/RenderHandles.h"

#include "RenderGraphHandle.h"
#include "RenderGraphResource.h"
#include "RenderGraphTexture.h"

namespace Horizon
{
	class RenderGraph;
	class PassNode;
	class RenderGraphVirtualResource;

	class RenderGraphRegistry
	{
	public:
		template<typename ResourceType>
		const ResourceType& GetResource(RenderGraphHandle<ResourceType> handle) const;
		template<typename ResourceType>
		const ResourceType::Description& GetResourceDescription(RenderGraphHandle<ResourceType> handle) const;
		template<typename ResourceType>
		const ResourceType::Description& GetSubresourceDescription(RenderGraphHandle<ResourceType> handle) const;
		template<typename ResourceType>
		ResourceBindFlags GetResourceBindFlags(RenderGraphHandle<ResourceType> handle) const;
		RenderTextureHandle GetTexture(RenderGraphHandle<RenderGraphTexture> handle) const { return GetResource<RenderGraphTexture>(handle).handle; }
	private:
		friend class RenderGraph;
		RenderGraphRegistry(RenderGraph* rg, PassNode* passNode) : mRenderGraph(rg), mPassNode(passNode) {}
		RenderGraphVirtualResource* GetResource_Internal(RenderGraphHandleBase handle) const;
		RenderGraph* mRenderGraph;
		PassNode* mPassNode;
	};

	template<typename ResourceType>
	inline const ResourceType& RenderGraphRegistry::GetResource(RenderGraphHandle<ResourceType> handle) const
	{
		return static_cast<RenderGraphResource<ResourceType>*>(GetResource_Internal(handle))->Get();
	}

	template<typename ResourceType>
	inline const ResourceType::Description& RenderGraphRegistry::GetResourceDescription(RenderGraphHandle<ResourceType> handle) const
	{
		return static_cast<RenderGraphResource<ResourceType>*>(GetResource_Internal(handle))->GetBindFlags();
	}

	template<typename ResourceType>
	inline const ResourceType::Description& RenderGraphRegistry::GetSubresourceDescription(RenderGraphHandle<ResourceType> handle) const
	{
		return static_cast<RenderGraphResource<ResourceType>*>(GetResource_Internal(handle))->GetBindFlags();
	}

	template<typename ResourceType>
	inline ResourceBindFlags RenderGraphRegistry::GetResourceBindFlags(RenderGraphHandle<ResourceType> handle) const
	{
		return static_cast<RenderGraphResource<ResourceType>*>(GetResource_Internal(handle))->GetBindFlags();
	}
}
