#pragma once

#include "Stage.h"

#include "Graph.h"

#include "ResourceManager.h"

#include "Gogaman/RenderhardwareInterface/RenderSurface.h"
#include "Gogaman/RenderhardwareInterface/ComputeState.h"
#include "Gogaman/RenderhardwareInterface/RenderState.h"
#include "Gogaman/RenderHardwareInterface/CommandBuffer.h"
#include "Gogaman/RenderHardwareInterface/CommandHeap.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		class Compiler;

		class ExecutableGraph
		{
		private:
			struct ExecutionData
			{
				Graph::StageIndex stageIndex;
				Stage::Type       stageType;
			};

			struct ExecutableStage
			{
				std::vector<std::string> inputTextureNames;
				std::vector<std::string> outputTextureNames;

				std::vector<std::string> inputBufferNames;
				std::vector<std::string> outputBufferNames;

				std::vector<std::string> stateUpdateTextureNames;

				Stage::ExecuteFunction execute;
			};

			struct ExecutableComputeStage : public ExecutableStage
			{
				ComputeStage::StateData stateData;

				std::unique_ptr<RHI::ComputeState> state;
			};

			struct ExecutablePrerecordedComputeStage : public ExecutableComputeStage
			{
				std::unique_ptr<RHI::CommandBuffer> commandBuffer;

				PrerecordedComputeStage::RecordCommandsFunction recordCommands;
			};

			struct ExecutableRenderStage : public ExecutableStage
			{
				std::vector<std::string> renderSurfaceAttachmentNames;

				RenderStage::StateData stateData;

				RHI::RenderSurfaceID renderSurfaceID;

				std::unique_ptr<RHI::RenderState> state;
			};

			struct ExecutablePrerecordedRenderStage : public ExecutableRenderStage
			{
				std::unique_ptr<RHI::CommandBuffer> commandBuffer;

				PrerecordedRenderStage::RecordCommandsFunction recordCommands;
			};
		public:
			ExecutableGraph()                   = default;
			ExecutableGraph(ExecutableGraph &&) = default;

			~ExecutableGraph();

			ExecutableGraph &operator=(ExecutableGraph &&) = default;

			void Initialize();

			void Execute();
		private:
			std::vector<ExecutionData> m_ExecutionOrder;

			std::vector<ExecutableComputeStage>            m_ComputeStages;
			std::vector<ExecutablePrerecordedComputeStage> m_PrerecordedComputeStages;
			std::vector<ExecutableRenderStage>             m_RenderStages;
			std::vector<ExecutablePrerecordedRenderStage>  m_PrerecordedRenderStages;

			std::unique_ptr<RHI::CommandHeap> m_ComputeCommandHeap, m_RenderCommandHeap;

			ResourceManager m_ResourceManager;

			std::vector<RHI::RenderState> lastStageStates;
		private:
			friend Compiler;
		};
	}
}