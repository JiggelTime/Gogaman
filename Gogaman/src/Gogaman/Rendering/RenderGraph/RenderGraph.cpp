#include "pch.h"
#include "RenderGraph.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		void Graph::SetBackBuffer(const std::string &name)
		{
			m_BackBuffer = name;
		}

		Stage &Graph::CreateStage()
		{
			static StageIndex nextIndex = 0;
			return m_Stages.emplace_back(Stage(nextIndex++));
		}

		void Graph::Compile()
		{
			for(TextureResource &i : m_TextureResources)
			{
				GM_ASSERT(!i.GetReadStages().empty() || i.GetName() == m_BackBuffer, "Failed to compile render graph | Resource \"%s\" is never used", i.GetName().c_str())
			}
			
			m_StageDependencies.resize(m_Stages.size());

			TextureResource &backBuffer = GetTextureResource(m_BackBuffer);
			GM_ASSERT(!backBuffer.GetWriteStages().empty(), "Failed to compile render graph | No stages write to back buffer")

			//Generate dependencies
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
			//Greedy allocation algorithm: last stage to use a physical resource is responsible for releasing it
			std::unordered_set<TextureResourcePhysicalID> visitedPhysicalIDs;
			for(auto j = m_StageSchedule.rbegin(); j != m_StageSchedule.rend(); j++)
			{
				Stage &stage = m_Stages[*j];
				for(TextureResourceIndex k : stage.GetReadTextureResources())
				{
					TextureResourcePhysicalID physicalID = m_TextureResources[k].GetPhysicalID();
					if(visitedPhysicalIDs.count(physicalID) == 0)
					{
						stage.AddDestroyTextureResource(k);
						visitedPhysicalIDs.emplace(physicalID);
					}
				}
			}
		}

		void Graph::GenerateDependencies(const Stage &stage)
		{
			std::vector<StageIndex> &dependencies = m_StageDependencies[stage.GetIndex()];
			for(TextureResourceIndex i : stage.GetReadTextureResources())
			{
				for(StageIndex j : m_TextureResources[i].GetWriteStages())
				{
					GM_ASSERT(j != stage.GetIndex(), "Failed to generate dependencies | Stage has self dependancy")

					m_StageSchedule.emplace_back(j);
					dependencies.emplace_back(j);

					GenerateDependencies(m_Stages[j]);
				}
			}
		}

		/*
		bool Graph::IsStagePrerequisite(const uint16_t sourceStageIndex, const uint16_t destinationStageIndex) const
		{
			if(sourceStageIndex == destinationStageIndex)
				return true;

			for(auto i : m_StageDependencies[destinationStageIndex])
			{
				if(IsStagePrerequisite(sourceStageIndex, i))
					return true;
			}

			return false;
		}*/

		void Graph::Log() const
		{
			GM_LOG_CORE_INFO("Render graph");
			for(StageIndex i : m_StageSchedule)
			{
				const Stage &stage = m_Stages[i];
				GM_LOG_CORE_INFO("	Stage %d", i);

				GM_LOG_CORE_INFO("	Reads:");
				for(TextureResourceIndex j :  stage.GetReadTextureResources())
				{
					const TextureResource &textureResource = m_TextureResources[j];
					GM_LOG_CORE_INFO("		%s", textureResource.GetName().c_str());
				}

				GM_LOG_CORE_INFO("	Writes:");
				for(TextureResourceIndex j :  stage.GetWriteTextureResources())
				{
					const TextureResource &textureResource = m_TextureResources[j];
					GM_LOG_CORE_INFO("		%s", textureResource.GetName().c_str());
				}
			}
		}

		TextureResource &Graph::GetTextureResource(const std::string &name)
		{
			GM_ASSERT(m_TextureResourceIndices.count(name) > 0, "Failed to get texture resource | Texture resource does not exist")
			return m_TextureResources[m_TextureResourceIndices[name]];
		}
	}
}