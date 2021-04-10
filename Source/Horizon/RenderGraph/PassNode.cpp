#include "PassNode.h"
#include "RenderGraph.h"

namespace Horizon
{ 
    PassNode::PassNode(RenderGraph* rg, const char* name)
        : RenderDependencyGraph::Node(rg->GetGraph(), name, RenderDependencyGraph::NodeType::PassNode)
        , mRenderGraph(rg)
    {

    }

    void PassNode::GraphVizify() const
    {

    }
}