#pragma once

#include "Stage.h"

#include "Graph.h"

#include "ResourceManager.h"

#include "Gogaman/RenderhardwareInterface/Identifier.h"
#include "Gogaman/RenderhardwareInterface/ComputeState.h"
#include "Gogaman/RenderhardwareInterface/RenderState.h"
#include "Gogaman/RenderHardwareInterface/CommandBuffer.h"

namespace Gogaman
{
	class FrameContext;

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

				std::vector<std::pair<std::string, RHI::Texture::State>> textureStateUpdates;

				Stage::ExecuteFunction execute;
			};

			struct ExecutableComputeStage : public ExecutableStage
			{
				ComputeStage::StateData stateData;

				std::unique_ptr<RHI::ComputeState> state;
			};

			struct ExecutableRenderStage : public ExecutableStage
			{
				std::vector<std::string> renderSurfaceAttachmentNames;

				RenderStage::StateData stateData;

				RHI::RenderSurfaceID renderSurfaceID;

				std::unique_ptr<RHI::RenderState> state;
			};
		public:
			ExecutableGraph()                   = default;
			ExecutableGraph(ExecutableGraph &&) = default;

			~ExecutableGraph();

			ExecutableGraph &operator=(ExecutableGraph &&) = default;

			void Initialize();

			void Execute(FrameContext &frameContext);
		private:
			std::vector<ExecutionData> m_ExecutionOrder;

			std::vector<ExecutableComputeStage> m_ComputeStages;
			std::vector<ExecutableRenderStage>  m_RenderStages;

			ResourceManager m_ResourceManager;

			std::vector<RHI::RenderState> lastStageStates;
		private:
			friend Compiler;
		};
	}
}