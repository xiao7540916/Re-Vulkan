#include "RenderGraphPassExecutor.h"
#include "PassNode.h"

namespace Horizon
{
    RenderGraphPassExecutor::RenderGraphPassExecutor(PassNode* node)
        : mNode(node)
    {
        node->BindPassExecutor(this);
    }

    RenderGraphRenderPassExecutor::RenderGraphRenderPassExecutor(PassNode* node, ExecuteCallback&& execute)
        : RenderGraphPassExecutor(node)
        , mExecuteCallback(std::move(execute)) 
    {
        
    }

    RenderGraphComputePassExecutor::RenderGraphComputePassExecutor(PassNode* node, ExecuteCallback&& execute)
        : RenderGraphPassExecutor(node)
        , mExecuteCallback(std::move(execute)) 
    {
        
    }
}