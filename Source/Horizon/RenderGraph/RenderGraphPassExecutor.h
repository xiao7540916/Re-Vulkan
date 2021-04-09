#pragma once

#include "Horizon/RenderBackend/RenderBackendCommon.h"

#include "RenderGraphRegistry.h"
#include "RenderGraphRenderPass.h"

namespace Horizon
{
    class PassNode;
    
    class RenderGraphPassExecutor 
    {
    public:
        using ResourceList = Vector<RenderGraphVirtualResource*>;
        RenderGraphPassExecutor(PassNode* node);
        virtual ~RenderGraphPassExecutor() = default;
        PassNode* GetNode() const { return mNode; }
        virtual bool IsAsynCompute() const { return false; }
        const ResourceList& GetDevirtualizeList() const { return mDevirtualizeList; }
        const ResourceList& GetDestroyList() const { return mDestroyList; }
        virtual void Execute(RenderGraphRegistry& registry, void* context) = 0;
    protected:
        friend class RenderGraph;
        PassNode* mNode;
		// Resources need to devirtualize before executing.
		ResourceList mDevirtualizeList;
		// Resources need to destroy after executing.
		ResourceList mDestroyList;
    };

    class RenderGraphRenderPassExecutor : public RenderGraphPassExecutor
    {
    public:
        using ExecuteCallback = std::function<void(RenderGraphRegistry&, RenderContext*)>;
        RenderGraphRenderPassExecutor(PassNode* node, ExecuteCallback&& execute);
        ~RenderGraphRenderPassExecutor() = default;
        void Execute(RenderGraphRegistry& registry, void* context) { mExecuteCallback(registry, (RenderContext*)context); }
    private:
        RenderGraphRenderPass::Description description;
        /// Input attachments.
        // TODO
        /// Output attachments.
        RenderGraphVirtualResource* outputAttachments[MaxSimultaneousColorAttachmentCount + 1] = {};
        ExecuteCallback mExecuteCallback;
    };

    class RenderGraphComputePassExecutor : public RenderGraphPassExecutor
    {
    public:
        using ExecuteCallback = std::function<void(RenderGraphRegistry&, ComputeContext*)>;
        RenderGraphComputePassExecutor(PassNode* node, ExecuteCallback&& execute);
        ~RenderGraphComputePassExecutor() = default;
        bool IsAsynCompute() const { return true; }
        void Execute(RenderGraphRegistry& registry, void* context) { mExecuteCallback(registry, (ComputeContext*)context); }
    private:
        ExecuteCallback mExecuteCallback;
    };
}
