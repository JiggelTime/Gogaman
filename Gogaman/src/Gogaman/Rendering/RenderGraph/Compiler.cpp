#include "pch.h"
#include "Compiler.h"

#include "Gogaman/Core/Base.h"

#include "Stage.h"

#include "Graph.h"

#include "ExecutableGraph.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		std::unique_ptr<ExecutableGraph> Compiler::Compile(Graph &&graph)
		{
			GM_DEBUG_ASSERT(!graph.m_OutputTextureName.empty(), "Failed to compile graph | Invalid graph output texture name");

			using StageIndex = Graph::StageIndex;

			DirectedAcyclicGraph<StageIndex> directedAcyclicGraph;

			//Create DAG vertices and find last stage
			std::optional<StageIndex> lastStageVertexIndex;
			auto CreateVertex = [&](const auto &stage)
			{
				const StageIndex index = directedAcyclicGraph.GetVertexCount();
				directedAcyclicGraph.CreateVertex();
				if(!lastStageVertexIndex.has_value())
				{
					for(const auto &i : stage.m_WriteTextureData)
					{
						if(i.outputName == graph.m_OutputTextureName)
						{
							lastStageVertexIndex = index;
							break;
						}
					}
				}
			};

			for(const auto &i : graph.m_ComputeStages)
			{
				CreateVertex(i);
			}

			for(const auto &i : graph.m_PrerecordedComputeStages)
			{
				CreateVertex(i);
			}

			for(const auto &i : graph.m_RenderStages)
			{
				CreateVertex(i);
			}

			for(const auto &i : graph.m_PrerecordedRenderStages)
			{
				CreateVertex(i);
			}

			//Find last stage
			auto FindLastStage = [&](const auto &stage, const StageIndex vertexIndex)
			{
				for(const auto &i : stage.m_CreateTextureData)
				{
					if(i.name == graph.m_OutputTextureName)
					{
						lastStageVertexIndex = vertexIndex;
						return;
					}
				}
			};

			for(StageIndex i = 0; i < graph.m_ComputeStages.size(); i++)
			{
				if(lastStageVertexIndex.has_value())
					break;

				FindLastStage(graph.m_ComputeStages[i], graph.VertexIndex(Stage::Type::Compute, i));
			}

			for(StageIndex i = 0; i < graph.m_PrerecordedComputeStages.size(); i++)
			{
				if(lastStageVertexIndex.has_value())
					break;

				FindLastStage(graph.m_PrerecordedComputeStages[i], graph.VertexIndex(Stage::Type::PrerecordedCompute, i));
			}

			for(StageIndex i = 0; i < graph.m_RenderStages.size(); i++)
			{
				if(lastStageVertexIndex.has_value())
					break;

				FindLastStage(graph.m_RenderStages[i], graph.VertexIndex(Stage::Type::Render, i));
			}

			for(StageIndex i = 0; i < graph.m_PrerecordedRenderStages.size(); i++)
			{
				if(lastStageVertexIndex.has_value())
					break;

				FindLastStage(graph.m_PrerecordedRenderStages[i], graph.VertexIndex(Stage::Type::PrerecordedRender, i));
			}

			GM_DEBUG_ASSERT(lastStageVertexIndex.has_value(), "Failed to compile graph | Output texture \"%s\" is not created by any stages", graph.m_OutputTextureName.c_str());
			
			//Process stage inputs/outputs
			std::unordered_map<std::string, StageIndex> resourceEdgeSourceVertexIndices;

			auto ProcessComputeStageOutputs = [&](const auto &stage, const StageIndex vertexIndex)
			{
				for(const auto &i : stage.m_CreateTextureData)
				{
					resourceEdgeSourceVertexIndices.emplace(i.name, vertexIndex);
				}

				for(const auto &i : stage.m_WriteTextureData)
				{
					resourceEdgeSourceVertexIndices.emplace(i.outputName, vertexIndex);
				}

				for(const auto &i : stage.m_CreateBufferData)
				{
					resourceEdgeSourceVertexIndices.emplace(i.name, vertexIndex);
				}

				for(const std::string &i : stage.m_WriteBufferOutputNames)
				{
					resourceEdgeSourceVertexIndices.emplace(i, vertexIndex);
				}
			};

			auto ProcessRenderStageOutputs = [&](auto &stage, const StageIndex vertexIndex)
			{
				for(const auto &i : stage.m_CreateTextureData)
				{
					resourceEdgeSourceVertexIndices.emplace(i.name, vertexIndex);
					if(i.state == RHI::Texture::State::RenderSurfaceAttachment)
						stage.m_RenderSurfaceAttachmentNames.emplace_back(i.name);
				}

				for(const auto &i : stage.m_WriteTextureData)
				{
					resourceEdgeSourceVertexIndices.emplace(i.outputName, vertexIndex);
					if(i.state == RHI::Texture::State::RenderSurfaceAttachment)
						stage.m_RenderSurfaceAttachmentNames.emplace_back(i.inputName);
				}

				for(const auto &i : stage.m_CreateBufferData)
				{
					resourceEdgeSourceVertexIndices.emplace(i.name, vertexIndex);
				}

				for(const std::string &i : stage.m_WriteBufferOutputNames)
				{
					resourceEdgeSourceVertexIndices.emplace(i, vertexIndex);
				}
			};
			
			for(StageIndex i = 0; i < graph.m_ComputeStages.size(); i++)
			{
				ProcessComputeStageOutputs(graph.m_ComputeStages[i], graph.VertexIndex(Stage::Type::Compute, i));
			}
			
			for(StageIndex i = 0; i < graph.m_PrerecordedComputeStages.size(); i++)
			{
				ProcessComputeStageOutputs(graph.m_PrerecordedComputeStages[i], graph.VertexIndex(Stage::Type::PrerecordedCompute, i));
			}

			for(StageIndex i = 0; i < graph.m_RenderStages.size(); i++)
			{
				ProcessRenderStageOutputs(graph.m_RenderStages[i], graph.VertexIndex(Stage::Type::Render, i));
			}

			for(StageIndex i = 0; i < graph.m_PrerecordedRenderStages.size(); i++)
			{
				ProcessRenderStageOutputs(graph.m_PrerecordedRenderStages[i], graph.VertexIndex(Stage::Type::PrerecordedRender, i));
			}

			auto ProcessStageInputs = [&](const auto &stage, const StageIndex vertexIndex)
			{
				for(const auto &i : stage.m_ReadTextureData)
				{
					GM_DEBUG_ASSERT(resourceEdgeSourceVertexIndices.find(i.name) != resourceEdgeSourceVertexIndices.end(), "Failed to compile graph | Texture \"%s\" is not created by any stages", i.name.c_str());

					const StageIndex sourceVertexIndex = resourceEdgeSourceVertexIndices[i.name];

					directedAcyclicGraph.CreateEdge(sourceVertexIndex, vertexIndex);

					const auto [sourceStageType, sourceStageIndex] = graph.StageIndexData(sourceVertexIndex);

					switch(sourceStageType)
					{
					case Stage::Type::Compute:
						graph.m_ComputeStages[sourceStageIndex].m_StateUpdateTextureNames.emplace_back(i.name);
					case Stage::Type::PrerecordedCompute:
						graph.m_PrerecordedComputeStages[sourceStageIndex].m_StateUpdateTextureNames.emplace_back(i.name);
					case Stage::Type::Render:
						graph.m_RenderStages[sourceStageIndex].m_StateUpdateTextureNames.emplace_back(i.name);
					case Stage::Type::PrerecordedRender:
						graph.m_PrerecordedRenderStages[sourceStageIndex].m_StateUpdateTextureNames.emplace_back(i.name);
					}
				}

				for(const auto &i : stage.m_WriteTextureData)
				{
					GM_DEBUG_ASSERT(resourceEdgeSourceVertexIndices.find(i.inputName) != resourceEdgeSourceVertexIndices.end(), "Failed to compile graph | Texture \"%s\" is not created by any stages", i.inputName.c_str());

					const StageIndex sourceVertexIndex = resourceEdgeSourceVertexIndices[i.inputName];

					directedAcyclicGraph.CreateEdge(sourceVertexIndex, vertexIndex);

					const auto [sourceStageType, sourceStageIndex] = graph.StageIndexData(sourceVertexIndex);

					switch(sourceStageType)
					{
					case Stage::Type::Compute:
						graph.m_ComputeStages[sourceStageIndex].m_StateUpdateTextureNames.emplace_back(i.inputName);
					case Stage::Type::PrerecordedCompute:
						graph.m_PrerecordedComputeStages[sourceStageIndex].m_StateUpdateTextureNames.emplace_back(i.inputName);
					case Stage::Type::Render:
						graph.m_RenderStages[sourceStageIndex].m_StateUpdateTextureNames.emplace_back(i.inputName);
					case Stage::Type::PrerecordedRender:
						graph.m_PrerecordedRenderStages[sourceStageIndex].m_StateUpdateTextureNames.emplace_back(i.inputName);
					}
				}

				for(const std::string &i : stage.m_ReadBufferNames)
				{
					GM_DEBUG_ASSERT(resourceEdgeSourceVertexIndices.find(i) != resourceEdgeSourceVertexIndices.end(), "Failed to compile graph | Buffer \"%s\" is not created by any stages", i.c_str());

					directedAcyclicGraph.CreateEdge(resourceEdgeSourceVertexIndices[i], vertexIndex);
				}

				for(const std::string &i : stage.m_WriteBufferInputNames)
				{
					GM_DEBUG_ASSERT(resourceEdgeSourceVertexIndices.find(i) != resourceEdgeSourceVertexIndices.end(), "Failed to compile graph | Buffer \"%s\" is not created by any stages", i.c_str());

					directedAcyclicGraph.CreateEdge(resourceEdgeSourceVertexIndices[i], vertexIndex);
				}
			};

			for(StageIndex i = 0; i < graph.m_ComputeStages.size(); i++)
			{
				ProcessStageInputs(graph.m_ComputeStages[i], graph.VertexIndex(Stage::Type::Compute, i));
			}

			for(StageIndex i = 0; i < graph.m_PrerecordedComputeStages.size(); i++)
			{
				ProcessStageInputs(graph.m_PrerecordedComputeStages[i], graph.VertexIndex(Stage::Type::PrerecordedCompute, i));
			}

			for(StageIndex i = 0; i < graph.m_RenderStages.size(); i++)
			{
				ProcessStageInputs(graph.m_RenderStages[i], graph.VertexIndex(Stage::Type::Render, i));
			}

			for(StageIndex i = 0; i < graph.m_PrerecordedRenderStages.size(); i++)
			{
				ProcessStageInputs(graph.m_PrerecordedRenderStages[i], graph.VertexIndex(Stage::Type::PrerecordedRender, i));
			}

			//Generate contributing vertex indices
			std::unordered_set<StageIndex> contributingVertexIndices;
			directedAcyclicGraph.DepthFirstTraversal<true>(lastStageVertexIndex.value(), [&contributingVertexIndices](const StageIndex vertexIndex)
			{
				contributingVertexIndices.emplace(vertexIndex);
			});

			std::vector<StageIndex> topologicalOrder = directedAcyclicGraph.TopologicalSort();

			//TODO: There might be duplicates from the topological sort (needs to be tested). Remove duplicates here.

			//Detect non-contributing stages and generate executable graph
			std::unique_ptr<ExecutableGraph> executable = std::make_unique<ExecutableGraph>();
			executable->m_ExecutionOrder.reserve(contributingVertexIndices.size());
			for(const StageIndex i : topologicalOrder)
			{
				GM_DEBUG_ASSERT(contributingVertexIndices.find(i) != contributingVertexIndices.end(), "Failed to compile graph | Invalid Stage | Stage does not contribute to graph output");

				auto CreateExecutableStage = [&executable](auto &executableStage, auto &stage, const StageIndex executionIndex)
				{
					for(const auto &i : stage.m_CreateTextureData)
					{
						executableStage.outputTextureNames.emplace_back(i.name);

						executable->m_ResourceManager.AddResource(i.name, i.texture, executionIndex);
					}

					for(const auto &i : stage.m_ReadTextureData)
					{
						executableStage.inputTextureNames.emplace_back(i.name);

						executable->m_ResourceManager.ExtendTextureLifetime(i.name, executionIndex);
					}

					for(const auto &i : stage.m_WriteTextureData)
					{
						executableStage.inputTextureNames.emplace_back(i.inputName);
						executableStage.outputTextureNames.emplace_back(i.outputName);

						executable->m_ResourceManager.ExtendTextureLifetime(i.inputName, executionIndex);
						executable->m_ResourceManager.AddAlias(i.inputName, i.outputName);
					}

					for(const auto &i : stage.m_CreateBufferData)
					{
						executableStage.outputBufferNames.emplace_back(i.name);

						executable->m_ResourceManager.AddResource(i.name, i.buffer, executionIndex);
					}

					for(const std::string &i : stage.m_ReadBufferNames)
					{
						executableStage.inputBufferNames.emplace_back(i);

						executable->m_ResourceManager.ExtendTextureLifetime(i, executionIndex);
					}

					//Remove this and just iterate through stage.m_WriteBufferData instead to get input/output names
					GM_DEBUG_ASSERT(stage.m_WriteBufferInputNames.size() == stage.m_WriteBufferOutputNames.size(), "Failed to compile graph | Invalid stage");

					for(auto i = 0; i < stage.m_WriteBufferInputNames.size(); i++)
					{
						const std::string inputName  = stage.m_WriteBufferInputNames[i];
						const std::string outputName = stage.m_WriteBufferOutputNames[i];

						executableStage.inputBufferNames.emplace_back(inputName);
						executableStage.outputBufferNames.emplace_back(outputName);

						executable->m_ResourceManager.ExtendTextureLifetime(inputName, executionIndex);
						executable->m_ResourceManager.AddAlias(inputName, outputName);
					}

					for(const std::string &i : stage.m_StateUpdateTextureNames)
					{
						executableStage.stateUpdateTextureNames.emplace_back(i);
					}

					executableStage.execute = std::move(stage.m_Execute);

					executableStage.stateData = std::move(stage.m_StateData);
				};

				auto CreateExecutableComputeStage = [&CreateExecutableStage](ExecutableGraph::ExecutableComputeStage &executableStage, ComputeStage &stage, const StageIndex executionIndex)
				{
					CreateExecutableStage(executableStage, stage, executionIndex);
				};

				auto CreateExecutableRenderStage = [&CreateExecutableStage](ExecutableGraph::ExecutableRenderStage &executableStage, RenderStage &stage, const StageIndex executionIndex)
				{
					CreateExecutableStage(executableStage, stage, executionIndex);

					for(const std::string &i : stage.m_RenderSurfaceAttachmentNames)
					{
						executableStage.renderSurfaceAttachmentNames.emplace_back(i);
					}
				};

				auto CreateExecutablePrerecordedComputeStage = [&CreateExecutableStage](ExecutableGraph::ExecutablePrerecordedComputeStage &executableStage, PrerecordedComputeStage &stage, const StageIndex executionIndex)
				{
					CreateExecutableStage(executableStage, stage, executionIndex);

					executableStage.recordCommands = std::move(stage.m_RecordCommands);
				};

				auto CreateExecutablePrerecordedRenderStage = [&CreateExecutableStage](ExecutableGraph::ExecutablePrerecordedRenderStage &executableStage, PrerecordedRenderStage &stage, const StageIndex executionIndex)
				{
					CreateExecutableStage(executableStage, stage, executionIndex);

					for(const std::string &i : stage.m_RenderSurfaceAttachmentNames)
					{
						executableStage.renderSurfaceAttachmentNames.emplace_back(i);
					}

					executableStage.recordCommands = std::move(stage.m_RecordCommands);
				};

				const auto [stageType, stageIndex] = graph.StageIndexData(i);
				switch(stageType)
				{
				case Stage::Type::Compute:
					CreateExecutableComputeStage(executable->m_ComputeStages.emplace_back(), graph.m_ComputeStages[stageIndex], i);
					break;
				case Stage::Type::Render:
					CreateExecutableRenderStage(executable->m_RenderStages.emplace_back(), graph.m_RenderStages[stageIndex], i);
					break;
				case Stage::Type::PrerecordedCompute:
					CreateExecutablePrerecordedComputeStage(executable->m_PrerecordedComputeStages.emplace_back(), graph.m_PrerecordedComputeStages[stageIndex], i);
					break;
				case Stage::Type::PrerecordedRender:
					CreateExecutablePrerecordedRenderStage(executable->m_PrerecordedRenderStages.emplace_back(), graph.m_PrerecordedRenderStages[stageIndex], i);
					break;
				}

				ExecutableGraph::ExecutionData &executionData = executable->m_ExecutionOrder.emplace_back();
				executionData.stageType  = stageType;
				executionData.stageIndex = stageIndex;
			}

			return executable;
		}
	}
}