#pragma once

#include "Horizon/Core/HorizonCommon.h"

namespace Horizon
{
	class RenderDependencyGraph
	{
	public:
		RenderDependencyGraph() = default;
		~RenderDependencyGraph() = default;
		enum class NodeType
		{
			PassNode,
			ResourceNode,
		};
		struct Edge;
		class Node
		{
		public:
			Node(RenderDependencyGraph* graph, const char* name, NodeType type) : mName(name), mType(type) { graph->RegisterNode(this); }
			virtual ~Node() = default;
			void Target() { mRefCount = kInfRefCount; }
			bool IsTarget() const { return mRefCount == kInfRefCount; }
			bool IsCulled() const { return mRefCount == 0; }
			char const* GetName() const { return mName; }
			uint32 GetRefCount() const { return mRefCount; }
			const Vector<Edge*>& GetIncomingEdges() const { return mIncomingEdges; }
			const Vector<Edge*>& GetOutgoingEdges() const { return mOutgoingEdges; }
			virtual void GraphVizify() const;
		protected:
			Vector<Edge*> mIncomingEdges;
			Vector<Edge*> mOutgoingEdges;
		private:
			friend class RenderDependencyGraph;
			static const uint32 kInfRefCount = (uint32)-1;
			uint32 mRefCount = 0;
			const char* mName;
			NodeType mType;
		};
		struct Edge
		{
			Node* const from;
			Node* const to;
			bool IsValid() const;
			virtual void GraphVizify() const;
			Edge(RenderDependencyGraph* graph, Node* from, Node* to) : from(from), to(to) { graph->RegisterEdge(this); }
		};
		void Cull();
		void Clear();
		const Vector<Edge*>& GetEdges() const { return mEdges; }
		const Vector<Node*>& GetNodes() const { return mNodes; }
		void GraphVizify() const;
	private:
		void RegisterNode(Node* node);
		void RegisterEdge(Edge* edge);
		Vector<Node*> mNodes;
		Vector<Edge*> mEdges;
	};
}
