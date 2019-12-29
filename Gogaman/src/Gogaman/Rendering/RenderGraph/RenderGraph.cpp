#include "pch.h"
#include "RenderGraph.h"

#include "RenderGraphStageBuilder.h"

#define GM_RENDER_GRAPH_DETECT_UNREFERENCED_RESOURCES 0
#define GM_RENDER_GRAPH_DETECT_UNREFERENCED_STAGES    0

namespace Gogaman
{
	namespace RenderGraph
	{
		void Graph::CreateStage(const Stage::SetupFunction &setupFunction, const Stage::ExecuteFunction &executeFunction)
		{
			static StageIndex nextIndex = 0;
			Stage &stage = m_Stages.emplace_back(Stage(nextIndex++, setupFunction, executeFunction));
			stage.Setup(StageBuilder(this, &stage));
		}

		void Graph::SetBackBuffer(const std::string &name)
		{
			m_BackBuffer = name;
		}

		void Graph::Compile()
		{
			#if GM_RENDER_GRAPH_DETECT_UNREFERENCED_RESOURCES
				for(const VirtualTexture &i : m_VirtualTextures)
				{
					GM_ASSERT(!i.GetReadStages().empty() || i.GetName() == m_BackBuffer, "Failed to compile render graph | Texture \"%s\" is unreferenced", i.GetName().c_str())
				}
			#endif

			#if GM_RENDER_GRAPH_DETECT_UNREFERENCED_STAGES
				for(const Stage &i : m_Stages)
				{
					GM_ASSERT(!i.GetWriteTextures().empty(), "Failed to compile render graph | Stage is unreferenced")
				}
			#endif

			m_StageDependencies.resize(m_Stages.size());

			const VirtualTexture &backBuffer = GetVirtualTexture(m_BackBuffer);
			GM_ASSERT(!backBuffer.GetWriteStages().empty(), "Failed to compile render graph | No stages write to back buffer")

			//Generate stage schedule
			for(StageIndex i : backBuffer.GetWriteStages())
			{
				m_StageSchedule.emplace_back(i);
			}

			auto backBufferWriteStages = m_StageSchedule;
			for(StageIndex i : backBufferWriteStages)
			{
				GenerateDependencies(m_Stages[i]);
			}

			std::reverse(m_StageSchedule.begin(), m_StageSchedule.end());

			//Remove duplicate stages
			auto iterator = m_StageSchedule.begin();
			std::unordered_set<StageIndex> visitedStages;
			for(auto i = m_StageSchedule.begin(); i != m_StageSchedule.end(); i++)
			{
				if(visitedStages.emplace(*i).second)
					*iterator++ = *i;
			}

			m_StageSchedule.erase(iterator, m_StageSchedule.end());

			//Reorder stage schedule
			/*
			std::vector<StageIndex> unscheduledStages;
			unscheduledStages.reserve(m_Stages.size());
			std::swap(m_StageSchedule, unscheduledStages);

			auto ScheduleStage = [&](const StageIndex index)
			{
				m_StageSchedule.emplace_back(unscheduledStages[index]);
				unscheduledStages.erase(unscheduledStages.begin() + index);
				//auto indexPosition = unscheduledStages.begin() + index;
				//std::move(indexPosition + 1, unscheduledStages.end(), indexPosition);
				//unscheduledStages.pop_back();
			};

			ScheduleStage(0);
			while(!unscheduledStages.empty())
			{
				StageIndex candidateStage = 0;
				int        largestOverlap = 0;
				for(StageIndex i = 0; i < unscheduledStages.size(); i++)
				{
					StageIndex stageIndex = unscheduledStages[i];

					int overlap = 0;
					for(auto iterator = m_StageSchedule.rbegin(); iterator != m_StageIndices.rend(); iterator++)
					{
						if(IsStagePrerequisite(*iterator, stageIndex))
							break;

						overlap++;
					}

					if(overlap <= largestOverlap)
						continue;

					bool breaksDependencies = false;
					for(StageIndex j = 0; j < i; j++)
					{
						if(IsStagePrerequisite(unscheduledStages[j], stageIndex))
						{
							breaksDependencies = true;
							break;
						}
					}

					if(breaksDependencies)
						continue;

					candidateStage = i;
					largestOverlap = overlap;
				}

				ScheduleStage(candidateStage);
			}*/

			//Calculate resource lifetimes
			//Greedy allocation algorithm | Last stage to use a physical resource is responsible for releasing it
			std::unordered_set<VirtualTextureID>       visitedTextureIDs;
			std::unordered_set<VirtualRenderSurfaceID> visitedRenderSurfaceIDs;
			for(auto i = m_StageSchedule.rbegin(); i != m_StageSchedule.rend(); i++)
			{
				Stage &stage = m_Stages[*i];
				for(VirtualTextureIndex j : stage.GetReadTextures())
				{
					VirtualTextureID identifier = m_VirtualTextures[j].GetID();
					if(visitedTextureIDs.count(identifier) == 0)
					{
						stage.AddDestroyTexture(j);
						visitedTextureIDs.emplace(identifier);
					}
				}

				#if !GM_RENDER_GRAPH_DETECT_UNREFERENCED_RESOURCES
					for(VirtualTextureIndex j : stage.GetCreateTextures())
					{
						VirtualTextureID identifier = m_VirtualTextures[j].GetID();
						if(visitedTextureIDs.count(identifier) == 0)
						{
							stage.AddDestroyTexture(j);
							visitedTextureIDs.emplace(identifier);
						}
					}
				#endif

				for(VirtualRenderSurfaceIndex j : stage.GetReadRenderSurfaces())
				{
					VirtualRenderSurfaceID identifier = m_VirtualRenderSurfaces[j].GetID();
					if(visitedRenderSurfaceIDs.count(identifier) == 0)
					{
						stage.AddDestroyRenderSurface(j);
						visitedRenderSurfaceIDs.emplace(identifier);
					}
				}
				
				for(VirtualRenderSurfaceIndex j : stage.GetCreateRenderSurfaces())
				{
					VirtualRenderSurfaceID identifier = m_VirtualRenderSurfaces[j].GetID();
					if(visitedRenderSurfaceIDs.count(identifier) == 0)
					{
						stage.AddDestroyRenderSurface(j);
						visitedRenderSurfaceIDs.emplace(identifier);
					}
				}
			}
		}

		void Graph::GenerateDependencies(const Stage &stage)
		{
			std::vector<StageIndex> &dependencies = m_StageDependencies[stage.GetIndex()];
			for(VirtualTextureIndex i : stage.GetReadTextures())
			{
				for(StageIndex j : m_VirtualTextures[i].GetWriteStages())
				{
					GM_ASSERT(j != stage.GetIndex(), "Failed to generate dependencies | Stage has self dependancy")

					m_StageSchedule.emplace_back(j);
					dependencies.emplace_back(j);

					GenerateDependencies(m_Stages[j]);
				}
			}
		}

		void Graph::Execute()
		{
			for(StageIndex i : m_StageSchedule)
			{
				Stage &stage = m_Stages[i];

				//Realize resources
				for(VirtualTextureIndex j : stage.GetCreateTextures())
				{
					VirtualTexture &virtualTexture = m_VirtualTextures[j];

					auto &physicalTexture = GM_RENDERING_CONTEXT.GetTexture2Ds().Create(m_VirtualTexturePhysicalIDs[virtualTexture.GetID()]);
					physicalTexture.internalFormat = virtualTexture.GetDescriptor().internalFormat;
					physicalTexture.format         = virtualTexture.GetDescriptor().format;
					physicalTexture.interpolation  = virtualTexture.GetDescriptor().interpolation;
					physicalTexture.levels         = virtualTexture.GetDescriptor().levels;
					physicalTexture.Generate(virtualTexture.GetDescriptor().width, virtualTexture.GetDescriptor().height);
				}

				for(VirtualRenderSurfaceIndex j : stage.GetCreateRenderSurfaces())
				{
					VirtualRenderSurface &virtualRenderSurface = m_VirtualRenderSurfaces[j];

					auto &physicalRenderSurface = GM_RENDERING_CONTEXT.GetRenderSurfaces().Create(m_VirtualRenderSurfacePhysicalIDs[virtualRenderSurface.GetID()]);
					for(const std::string &k : virtualRenderSurface.GetDescriptor().renderTargets)
					{
						VirtualTexture &virtualRenderTarget = GetVirtualTexture(k);

						auto &physicalRenderTarget = GM_RENDERING_CONTEXT.GetTexture2Ds().Get(m_VirtualTexturePhysicalIDs[virtualRenderTarget.GetID()]);
						if(Texture::IsInternalFormatDepth(virtualRenderTarget.GetDescriptor().internalFormat))
							physicalRenderSurface.AddDepthBuffer(physicalRenderTarget);
						else
							physicalRenderSurface.AddColorBuffer(physicalRenderTarget);
					}
				}

				stage.Execute();

				//Derealize resources
				for(VirtualTextureIndex j : stage.GetDestroyTextures())
				{
					GM_RENDERING_CONTEXT.GetTexture2Ds().Erase(m_VirtualTexturePhysicalIDs[m_VirtualTextures[j].GetID()]);
				}

				for(VirtualRenderSurfaceIndex j : stage.GetDestroyRenderSurfaces())
				{
					GM_RENDERING_CONTEXT.GetRenderSurfaces().Erase(m_VirtualRenderSurfacePhysicalIDs[m_VirtualRenderSurfaces[j].GetID()]);
				}
			}
		}

		void Graph::Log() const
		{
			for(StageIndex i : m_StageSchedule)
			{
				const Stage &stage = m_Stages[i];
				GM_LOG_CORE_INFO("[RenderGraph] Stage %d", i);

				auto LogVirtualTexture = [&](const VirtualTextureIndex index)
				{
					const VirtualTexture &virtualTexture = m_VirtualTextures[index];
					GM_LOG_CORE_INFO("[RenderGraph] 		[Texture] %s", virtualTexture.GetName().c_str());
				};

				auto LogVirtualRenderSurface = [&](const VirtualRenderSurfaceIndex index)
				{
					const VirtualRenderSurface &virtualRenderSurface = m_VirtualRenderSurfaces[index];
					GM_LOG_CORE_INFO("[RenderGraph] 		[RenderSurface] %s", virtualRenderSurface.GetName().c_str());
				};

				GM_LOG_CORE_INFO("[RenderGraph] 	Creates:");
				for(VirtualTextureIndex j : stage.GetCreateTextures())
				{
					LogVirtualTexture(j);
				}

				for(VirtualRenderSurfaceIndex j : stage.GetCreateRenderSurfaces())
				{
					LogVirtualRenderSurface(j);
				}

				GM_LOG_CORE_INFO("[RenderGraph] 	Reads:");
				for(VirtualTextureIndex j :  stage.GetReadTextures())
				{
					LogVirtualTexture(j);
				}

				for(VirtualRenderSurfaceIndex j : stage.GetReadRenderSurfaces())
				{
					LogVirtualRenderSurface(j);
				}

				GM_LOG_CORE_INFO("[RenderGraph] 	Writes:");
				for(VirtualTextureIndex j :  stage.GetWriteTextures())
				{
					LogVirtualTexture(j);
				}
				
				GM_LOG_CORE_INFO("[RenderGraph] 	Destroys:");
				for (VirtualTextureIndex j : stage.GetDestroyTextures())
				{
					LogVirtualTexture(j);
				}

				for (VirtualRenderSurfaceIndex j : stage.GetDestroyRenderSurfaces())
				{
					LogVirtualRenderSurface(j);
				}
			}
		}

		VirtualTexture &Graph::GetVirtualTexture(const std::string &name)
		{
			GM_ASSERT(m_VirtualTextureIndices.count(name) > 0, "Failed to get virtual texture | Name \"%s\" is invalid", name.c_str())
			return m_VirtualTextures[m_VirtualTextureIndices[name]];
		}

		VirtualRenderSurface &Graph::GetVirtualRenderSurface(const std::string &name)
		{
			GM_ASSERT(m_VirtualRenderSurfaceIndices.count(name) > 0, "Failed to get virtual render surface | Name \"%s\" is invalid", name.c_str())
				return m_VirtualRenderSurfaces[m_VirtualRenderSurfaceIndices[name]];
		}
	}
}