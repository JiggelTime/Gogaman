#include "pch.h"
#include "RenderGraph.h"

#include "RenderGraphStageBuilder.h"

#include "Gogaman/Application.h"

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
			for(const VirtualTexture &i : m_VirtualTextures)
			{
				GM_ASSERT(!i.GetReadStages().empty() || i.GetName() == m_BackBuffer, "Failed to compile render graph | Resource \"%s\" is never used", i.GetName().c_str())
			}
			
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
			std::unordered_set<VirtualTextureID> visitedIDs;
			for(auto j = m_StageSchedule.rbegin(); j != m_StageSchedule.rend(); j++)
			{
				Stage &stage = m_Stages[*j];
				for(VirtualTextureIndex k : stage.GetReadTextures())
				{
					VirtualTextureID identifier = m_VirtualTextures[k].GetID();
					if(visitedIDs.count(identifier) == 0)
					{
						stage.AddDestroyTexture(k);
						visitedIDs.emplace(identifier);
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

				stage.Execute();

				//Derealize resources
				for(VirtualTextureIndex j : stage.GetDestroyTextures())
				{
					GM_RENDERING_CONTEXT.GetTexture2Ds().Erase(m_VirtualTexturePhysicalIDs[m_VirtualTextures[j].GetID()]);
				}
			}
		}

		void Graph::Log() const
		{
			GM_LOG_CORE_INFO("Render graph");
			for(StageIndex i : m_StageSchedule)
			{
				const Stage &stage = m_Stages[i];
				GM_LOG_CORE_INFO("Stage %d", i);

				auto LogVirtualTexture = [&](const VirtualTextureIndex index)
				{
					const VirtualTexture &virtualTexture = m_VirtualTextures[index];
					GM_LOG_CORE_INFO("		%s", virtualTexture.GetName().c_str());
				};

				GM_LOG_CORE_INFO("	Creates:");
				for (VirtualTextureIndex j : stage.GetCreateTextures())
				{
					LogVirtualTexture(j);
				}

				GM_LOG_CORE_INFO("	Reads:");
				for(VirtualTextureIndex j :  stage.GetReadTextures())
				{
					LogVirtualTexture(j);
				}

				GM_LOG_CORE_INFO("	Writes:");
				for(VirtualTextureIndex j :  stage.GetWriteTextures())
				{
					LogVirtualTexture(j);
				}
				
				GM_LOG_CORE_INFO("	Destroys:");
				for (VirtualTextureIndex j : stage.GetDestroyTextures())
				{
					LogVirtualTexture(j);
				}
			}
		}

		VirtualTexture &Graph::GetVirtualTexture(const std::string &name)
		{
			GM_ASSERT(m_VirtualTextureIndices.count(name) > 0, "Failed to get texture resource | Texture resource does not exist")
			return m_VirtualTextures[m_VirtualTextureIndices[name]];
		}
	}
}