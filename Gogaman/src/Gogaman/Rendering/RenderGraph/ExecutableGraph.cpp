#include "pch.h"
#include "ExecutableGraph.h"

#include "Gogaman/RenderHardwareInterface/ComputeCommandRecorder.h"
#include "Gogaman/RenderHardwareInterface/RenderCommandRecorder.h"
#include "Gogaman/RenderhardwareInterface/Device.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		ExecutableGraph::~ExecutableGraph()
		{
			for(auto &i : m_ComputeStages)
			{
				i.state.reset();
			}

			for(auto &i : m_RenderStages)
			{
				g_Device->GetResources().renderSurfaces.Destroy(i.renderSurfaceID);
				i.state.reset();
			}

			for(auto &i : m_PrerecordedComputeStages)
			{
				i.commandBuffer.reset();

				i.state.reset();
			}

			for(auto &i : m_PrerecordedRenderStages)
			{
				i.commandBuffer.reset();

				g_Device->GetResources().renderSurfaces.Destroy(i.renderSurfaceID);
				i.state.reset();
			}

			m_ComputeCommandHeap.reset();
			m_RenderCommandHeap.reset();
		}

		void ExecutableGraph::Initialize()
		{
			m_ResourceManager.Initialize();

			m_ComputeCommandHeap = std::make_unique<RHI::CommandHeap>(RHI::CommandHeap::Type::Compute);
			m_RenderCommandHeap  = std::make_unique<RHI::CommandHeap>(RHI::CommandHeap::Type::Render);

			auto InitializeComputeStage = [this](auto &stage)
			{
				stage.state = std::make_unique<RHI::ComputeState>(stage.stateData.descriptorGroupLayouts, stage.stateData.shaderProgramID);
			};

			auto InitializePrerecordedComputeStage = [&](ExecutablePrerecordedComputeStage &stage)
			{
				InitializeComputeStage(stage);

				stage.commandBuffer = m_ComputeCommandHeap->CreateReusableCommandBuffer();

				RHI::ComputeCommandRecorder commandRecorder(stage.commandBuffer.get(), stage.state.get());

				stage.recordCommands(commandRecorder, stage.stateData);
				//Inject barriers here?
				commandRecorder.StopRecording();
			};

			auto InitializeRenderStage = [&](auto &stage)
			{
				std::vector<RHI::RenderSurface::Attachment> colorAttachments;
				RHI::RenderSurface::Attachment              depthStencilAttachment;
				for(const std::string &i : stage.renderSurfaceAttachmentNames)
				{
					const RHI::TextureID textureID = m_ResourceManager.GetTextureID(i);
					const RHI::Texture &texture = g_Device->GetResources().textures.Get(textureID);

					const RHI::Texture::BaseFormat baseFormat = RHI::Texture::GetBaseFormat(texture.GetFormat());
					if((baseFormat == RHI::Texture::BaseFormat::Depth) || (baseFormat == RHI::Texture::BaseFormat::DepthStencil))
						depthStencilAttachment.textureID = textureID;
					else
					{
						auto &colorAttachment = colorAttachments.emplace_back();
						colorAttachment.textureID = textureID;
					}
				}

				stage.renderSurfaceID = g_Device->GetResources().renderSurfaces.Create(colorAttachments.size(), colorAttachments.data(), std::move(depthStencilAttachment), stage.stateData.viewportWidth, stage.stateData.viewportHeight, 1);

				stage.state = std::make_unique<RHI::RenderState>(stage.stateData.descriptorGroupLayouts, stage.stateData.vertexLayout, stage.stateData.shaderProgramID, stage.renderSurfaceID, stage.stateData.depthStencilState, stage.stateData.blendState, stage.stateData.viewportWidth, stage.stateData.viewportHeight, stage.stateData.cullState);
			};

			auto InitializePrerecordedRenderStage = [&](ExecutablePrerecordedRenderStage &stage)
			{
				InitializeRenderStage(stage);

				stage.commandBuffer = m_RenderCommandHeap->CreateReusableCommandBuffer();

				RHI::RenderCommandRecorder commandRecorder(stage.commandBuffer.get(), stage.state.get());

				stage.recordCommands(commandRecorder, stage.stateData);
				//Inject barriers here?
				commandRecorder.StopRecording();
			};

			for(const auto i : m_ExecutionOrder)
			{
				switch(i.stageType)
				{
				case Stage::Type::Compute:
					InitializeComputeStage(m_ComputeStages[i.stageIndex]);
					break;
				case Stage::Type::Render:
					InitializeRenderStage(m_RenderStages[i.stageIndex]);
					break;
				case Stage::Type::PrerecordedCompute:
					InitializePrerecordedComputeStage(m_PrerecordedComputeStages[i.stageIndex]);
					break;
				case Stage::Type::PrerecordedRender:
					InitializePrerecordedRenderStage(m_PrerecordedRenderStages[i.stageIndex]);
					break;
				}
			}
		}
	
		void ExecutableGraph::Execute()
		{
			auto ExecuteComputeStage = [this](ExecutableComputeStage &stage)
			{
				//Submit a command buffer that has the stage's barriers?
				stage.execute(m_ResourceManager);
			};

			auto ExecuteRenderStage = [this](ExecutableRenderStage &stage)
			{
				//Submit a command buffer that has the stage's barriers?
				stage.execute(m_ResourceManager);
			};

			auto ExecutePrerecordedComputeStage = [this](ExecutablePrerecordedComputeStage &stage)
			{
				stage.execute(m_ResourceManager);
				g_Device->SubmitComputeCommands(1, stage.commandBuffer.get());
			};

			auto ExecutePrerecordedRenderStage = [this](ExecutablePrerecordedRenderStage &stage)
			{
				stage.execute(m_ResourceManager);
				g_Device->SubmitRenderCommands(1, stage.commandBuffer.get());
			};

			for(ExecutionData i : m_ExecutionOrder)
			{
				switch(i.stageType)
				{
				case Stage::Type::Compute:
					ExecuteComputeStage(m_ComputeStages[i.stageIndex]);
					break;
				case Stage::Type::Render:
					ExecuteRenderStage(m_RenderStages[i.stageIndex]);
					break;
				case Stage::Type::PrerecordedCompute:
					ExecutePrerecordedComputeStage(m_PrerecordedComputeStages[i.stageIndex]);
					break;
				case Stage::Type::PrerecordedRender:
					ExecutePrerecordedRenderStage(m_PrerecordedRenderStages[i.stageIndex]);
					break;
				}
			}
		}
	}
}