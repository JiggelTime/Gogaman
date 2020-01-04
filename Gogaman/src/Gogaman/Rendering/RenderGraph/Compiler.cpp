#include "pch.h"
#include "Compiler.h"

#include "Gogaman/Core/Base.h"

#include "Stage.h"

#include "StageBuilder.h"

#include "ExecutableGraph.h"

#include "Graph.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		const ExecutableGraph &Compiler::Compile(Graph &&graph)
		{
			//Generate resource source stages
			std::unordered_map<std::string, Stage::Index> resourceSourceStages;
			for(Stage &i : graph.m_Stages)
			{
				i.Initialize(StageBuilder(&i));

				for(const Texture &j : i.m_CreateTextures)
				{
					resourceSourceStages.emplace(j.name, i.GetIndex());
				}

				for(const std::string &j : i.m_WriteTextureOutputNames)
				{
					resourceSourceStages.emplace(j, i.GetIndex());
				}
			}
			
			//Generate edges
			for(Stage &i : graph.m_Stages)
			{
				for(const std::string &j : i.m_ReadTextureNames)
				{
					GM_DEBUG_ASSERT(resourceSourceStages.find(j) != resourceSourceStages.end(), "Failed to compile graph | Texture \"%s\" is not created by any stages");

					graph.m_DirectedGraph.CreateEdge(resourceSourceStages[j], i.GetIndex());
				}

				for(const std::string &j : i.m_WriteTextureInputNames)
				{
					GM_DEBUG_ASSERT(resourceSourceStages.find(j) != resourceSourceStages.end(), "Failed to compile graph | Texture \"%s\" is not created by any stages");

					graph.m_DirectedGraph.CreateEdge(resourceSourceStages[j], i.GetIndex());
				}

				for(const Buffer &j : i.m_CreateBuffers)
				{
					resourceSourceStages.emplace(j.name, i.GetIndex());
				}

				for(const std::string &j : i.m_ReadBufferNames)
				{
					GM_DEBUG_ASSERT(resourceSourceStages.find(j) != resourceSourceStages.end(), "Failed to compile graph | Buffer \"%s\" is not created by any stages");

					graph.m_DirectedGraph.CreateEdge(resourceSourceStages[j], i.GetIndex());
				}

				for(const std::string &j : i.m_WriteBufferInputNames)
				{
					GM_DEBUG_ASSERT(resourceSourceStages.find(j) != resourceSourceStages.end(), "Failed to compile graph | Buffer \"%s\" is not created by any stages");

					graph.m_DirectedGraph.CreateEdge(resourceSourceStages[j], i.GetIndex());
				}
			}

			//DIRECTED GRAPH HAS NOW BEEN BUILT

			//Resolve execution order
			std::vector<Stage::Index> executionOrder;

			std::vector<Stage> executionQueue;
			for(const auto i : executionOrder)
			{
				executionQueue.emplace_back(std::move(graph.m_Stages[i]));
			}
			ExecutableGraph executable(std::move(executionQueue));
			return executable;
		}
	}
}