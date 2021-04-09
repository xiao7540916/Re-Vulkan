#include "RenderGraphRegistry.h"
#include "RenderGraph.h"

namespace Horizon
{
    RenderGraphVirtualResource* RenderGraphRegistry::GetResource(RenderGraphHndlae handle) const
    {
        return mRenderGraph->GetResource(handle);
    }
}