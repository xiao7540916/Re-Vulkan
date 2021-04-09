#include "RenderDependencyGraph.h"

namespace Horizon
{
	void RenderDependencyGraph::Cull()
	{
		auto& nodes = mNodes;
		auto& edges = mEdges;

		// Compute initial resource and pass reference counts.
		for (Edge* edge : edges) 
		{
			edge->from->mRefCount++;
		}

		// Push nodes with a 0 reference count on a stack.
		Vector<Node*> stack;
		for (Node* node : nodes)
		{
			if (node->GetRefCount() == 0) 
			{
				stack.push_back(node);
			}
		}

		while (!stack.empty()) 
		{
			Node* node = stack.back();
			stack.pop_back();
			for (Edge* incomingEdge : node->mIncomingEdges)
			{
				Node* from = incomingEdge->from;
				from->mRefCount--;
				if (from->GetRefCount() == 0)
				{
					stack.push_back(from);
				}
			}
		}
	}

	void RenderDependencyGraph::Clear()
	{
		mEdges.clear();
		mNodes.clear();
	}

	void RenderDependencyGraph::RegisterNode(Node* node)
	{
		mNodes.push_back(node);
	}

	void RenderDependencyGraph::RegisterEdge(Edge* edge)
	{
		edge->from->mOutgoingEdges.push_back(edge);
		edge->to->mIncomingEdges.push_back(edge);
		mEdges.push_back(edge);
	}

	bool RenderDependencyGraph::Edge::IsValid() const
	{
		return (!from->IsCulled()) && !(to->IsCulled());
	}

	void RenderDependencyGraph::Node::Graphvizify() const
	{

	}

	void RenderDependencyGraph::Graphvizify() const
	{
		
	}
}