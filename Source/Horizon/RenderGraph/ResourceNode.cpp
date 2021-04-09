#include "ResourceNode.h"
#include "RenderGraph.h"

namespace Horizon
{
    ResourceNode::ResourceNode(RenderGraph* rg, const char* name, uint64 index, uint64 parentIndex)
        : RenderDependencyGraph::Node(rg->GetGraph(), name, RenderDependencyGraph::NodeType::ResourceNode)
        , mRenderGraph(rg)
        , mIndex(index)
        , mParentIndex(parentIndex) 
    {
        
    }

    ResourceNode::~ResourceNode()
    {
        
    }

    void ResourceNode::AddReader(PassNode* passNode)
    {
        mReaders.push_back(passNode);
    }

    void ResourceNode::SetWriter(PassNode* passNode)
    {
        mWriter = passNode;
    }

    ResourceNode* ResourceNode::GetParentNode() const
    {
        return mRenderGraph->GetResouceNode(mParentIndex);
    }

    void ResourceNode::Graphvizify() const
    {
        
    }
}
