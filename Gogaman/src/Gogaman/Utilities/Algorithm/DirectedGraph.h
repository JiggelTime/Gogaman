#pragma once

#include "Gogaman/Core/Base.h"

namespace Gogaman
{
	template<typename VertexIndexType>
	class DirectedGraph
	{
	public:
		DirectedGraph()  = default;
		~DirectedGraph() = default;

		inline VertexIndexType CreateVertex()
		{
			GM_DEBUG_ASSERT(m_AdjacencyMatrix.size() < std::numeric_limits<VertexIndexType>::max(), "Failed to create vertex | Graph size exceeds %d", std::numeric_limits<VertexIndexType>::max() - 1);

			const VertexIndexType index = m_AdjacencyMatrix.size();

			for(VertexIndexType i = 0; i < m_AdjacencyMatrix.size(); i++)
				m_AdjacencyMatrix[i].emplace_back(false);

			m_AdjacencyMatrix.emplace_back().assign(m_AdjacencyMatrix.size(), false);

			return index;
		}

		/*
		//Does not work | Indices get shifted when vertex is deleted | Needs stable array
		inline void RemoveVertex(const VertexIndexType index)
		{
			for(VertexIndexType i = 0; i < m_AdjacencyMatrix.size(); i++)
			{
				m_AdjacencyMatrix[i].erase(m_AdjacencyMatrix[i].begin() + index);
			}

			m_AdjacencyMatrix[index].clear();
		}*/

		inline void CreateEdge(const VertexIndexType source, const VertexIndexType destination) { m_AdjacencyMatrix[source][destination] = true; }
		inline bool IsEdge(const VertexIndexType source, const VertexIndexType destination) const { return m_AdjacencyMatrix[source][destination]; }
		inline void RemoveEdge(const VertexIndexType source, const VertexIndexType destination) { m_AdjacencyMatrix[source][destination] = false; }

		inline std::vector<VertexIndexType> IncomingConnections(const VertexIndexType vertexIndex) const
		{
			std::vector<VertexIndexType> incomingEdges;
			for(VertexIndexType i = 0; i < m_AdjacencyMatrix.size(); i++)
			{
				if(m_AdjacencyMatrix[i][vertexIndex])
					incomingEdges.emplace_back(i);
			}

			return incomingEdges;
		}

		inline std::vector<VertexIndexType> OutgoingConnections(const VertexIndexType vertexIndex) const
		{
			std::vector<VertexIndexType> outgoingEdges;
			for(VertexIndexType i = 0; i < m_AdjacencyMatrix.size(); i++)
			{
				if(m_AdjacencyMatrix[vertexIndex][i])
					outgoingEdges.emplace_back(i);
			}

			return outgoingEdges;
		}

		inline VertexIndexType IncomingDegree(const VertexIndexType vertexIndex) const
		{
			VertexIndexType incomingDegree = 0;
			for(const auto &i : m_AdjacencyMatrix)
			{
				if(i[vertexIndex])
					incomingDegree++;
			}

			return incomingDegree;
		}

		inline VertexIndexType OutgoingDegree(const VertexIndexType vertexIndex) const
		{
			VertexIndexType outgoingDegree = 0;
			for(const bool i : m_AdjacencyMatrix[vertexIndex])
			{
				if(i)
					outgoingDegree++;
			}

			return outgoingDegree;
		}

		inline std::vector<VertexIndexType> PreOrder(const VertexIndexType start) const
		{
			std::vector<VertexIndexType> preOrder;
			preOrder.reserve(GetVertexCount());

			std::stack<VertexIndexType> vertices;
			vertices.emplace(start);
			std::vector<bool> discoveredVertexLabels(GetVertexCount(), false);
			while(!vertices.empty())
			{
				const VertexIndexType vertex = vertices.top();
				vertices.pop();

				preOrder.emplace_back(vertex);
				
				if(!discoveredVertexLabels[vertex])
				{
					discoveredVertexLabels[vertex] = true;
					for(const VertexIndexType i : OutgoingConnections(vertex))
						vertices.emplace(i);
				}
			}

			return preOrder;
		}

		inline std::vector<VertexIndexType> PostOrder(const VertexIndexType start) const
		{
			std::vector<VertexIndexType> postOrder;
			postOrder.reserve(GetVertexCount());

			std::stack<VertexIndexType> vertices;
			vertices.emplace(start);
			std::vector<bool> discoveredVertexLabels(GetVertexCount(), false);
			discoveredVertexLabels[start] = true;
			while(!vertices.empty())
			{
				const VertexIndexType vertex = vertices.top();
				bool isTail = true;

				for(const VertexIndexType i : OutgoingConnections(vertex))
				{
					if(!discoveredVertexLabels[i])
					{
						vertices.emplace(i);
						discoveredVertexLabels[i] = true;
						isTail = false;
						break;
					}
				}

				if(isTail)
				{
					vertices.pop();
					postOrder.emplace_back(vertex);
				}
			}

			return postOrder;
		}

		inline constexpr VertexIndexType GetVertexCount() const { return m_AdjacencyMatrix.size(); }
	private:
		std::vector<std::vector<bool>> m_AdjacencyMatrix;
	};

	template<typename VertexIndexType>
	class DirectedAcyclicGraph : public DirectedGraph<VertexIndexType>
	{
	public:
		DirectedAcyclicGraph()  = default;
		~DirectedAcyclicGraph() = default;

		inline bool Validate(const VertexIndexType rootIndex) const
		{
			std::stack<VertexIndexType> vertices;
			vertices.emplace(rootIndex);

			while(!vertices.empty())
			{
				const VertexIndexType vertex = vertices.top();
				vertices.pop();

				const std::vector<VertexIndexType> adjacentVertices = DirectedGraph<VertexIndexType>::OutgoingConnections(vertex);
				for(const VertexIndexType i : adjacentVertices)
					vertices.emplace(i);

				if(!vertices.empty() && vertices.top() == rootIndex)
					return false;
			}

			return true;
		}

		template<bool isReversed = false>
		inline void DepthFirstTraversal(const VertexIndexType rootIndex, std::function<void(const VertexIndexType)> callback) const
		{
			GM_DEBUG_ASSERT(Validate(rootIndex), "Depth first traversal failed | Graph is not acyclic");

			std::stack<VertexIndexType> vertices;
			vertices.emplace(rootIndex);

			while(!vertices.empty())
			{
				const VertexIndexType vertex = vertices.top();

				callback(vertex);

				vertices.pop();

				const std::vector<VertexIndexType> adjacentVertices = isReversed ? DirectedGraph<VertexIndexType>::IncomingConnections(vertex) : DirectedGraph<VertexIndexType>::OutgoingConnections(vertex);
				for(const VertexIndexType i : adjacentVertices)
					vertices.emplace(i);
			}
		}

		template<bool isReversed = false>
		inline void BreadthFirstTraversal(const VertexIndexType rootIndex, std::function<void(const VertexIndexType)> callback) const
		{
			GM_DEBUG_ASSERT(Validate(rootIndex), "Breadth first traversal failed | Graph is not acyclic");

			std::queue<VertexIndexType> vertices;
			vertices.emplace(rootIndex);

			while(!vertices.empty())
			{
				const VertexIndexType vertex = vertices.front();

				callback(vertex);

				vertices.pop();

				const std::vector<VertexIndexType> adjacentVertices = isReversed ? DirectedGraph<VertexIndexType>::IncomingConnections(vertex) : DirectedGraph<VertexIndexType>::OutgoingConnections(vertex);
				for(const VertexIndexType i : adjacentVertices)
					vertices.emplace(i);
			}
		}
	
		//Kahn's algorithm
		inline std::vector<VertexIndexType> TopologicalSort() const
		{
			std::vector<VertexIndexType> vertexIncomingDegrees(DirectedGraph<VertexIndexType>::GetVertexCount(), 0);
			std::queue<VertexIndexType>  zeroIncomingDegreeVertices;
			for(VertexIndexType i = 0; i < DirectedGraph<VertexIndexType>::GetVertexCount(); i++)
			{
				vertexIncomingDegrees[i] = DirectedGraph<VertexIndexType>::IncomingDegree(i);

				if(vertexIncomingDegrees[i] == 0)
					zeroIncomingDegreeVertices.emplace(i);
			}

			VertexIndexType              visitedVertexCount = 0;
			std::vector<VertexIndexType> topologicalOrder;
			while(!zeroIncomingDegreeVertices.empty())
			{
				VertexIndexType index = zeroIncomingDegreeVertices.front();
				topologicalOrder.emplace_back(index);

				zeroIncomingDegreeVertices.pop();

				for(const VertexIndexType i : DirectedGraph<VertexIndexType>::OutgoingConnections(index))
				{
					if(!--vertexIncomingDegrees[i])
						zeroIncomingDegreeVertices.emplace(i);
				}
				
				visitedVertexCount++;
			}

			GM_DEBUG_ASSERT(visitedVertexCount == DirectedGraph<VertexIndexType>::GetVertexCount(), "Topological sort failed | Graph is not acyclic");

			return topologicalOrder;
		}
	};
}