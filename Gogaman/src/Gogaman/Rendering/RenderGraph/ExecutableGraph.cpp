#include "pch.h"
#include "ExecutableGraph.h"

#include "Gogaman/RenderhardwareInterface/RenderSurface.h"
#include "Gogaman/RenderHardwareInterface/ComputeCommandRecorder.h"
#include "Gogaman/RenderHardwareInterface/RenderCommandRecorder.h"
#include "Gogaman/RenderhardwareInterface/Device.h"

#include "Gogaman/Rendering/FrameContext.h"

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
		}

		void ExecutableGraph::Initialize()
		{
			m_ResourceManager.Initialize();

			auto InitializeComputeStage = [this](auto &stage)
			{
				stage.state = std::make_unique<RHI::ComputeState>(stage.stateData.descriptorGroupLayouts, stage.stateData.shaderProgramID);
			};

			auto InitializeRenderStage = [&](auto &stage)
			{
				std::vector<RHI::RenderSurface::Attachment> *colorAttachments = new std::vector<RHI::RenderSurface::Attachment>;
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
						auto &colorAttachment = colorAttachments->emplace_back();
						colorAttachment.textureID = textureID;
					}
				}

				stage.renderSurfaceID = g_Device->GetResources().renderSurfaces.Create(colorAttachments->size(), colorAttachments->data(), std::move(depthStencilAttachment), stage.stateData.viewportWidth, stage.stateData.viewportHeight, 1);

				stage.state = std::make_unique<RHI::RenderState>(stage.stateData.descriptorGroupLayouts, *stage.stateData.vertexLayout.get(), stage.stateData.shaderProgramID, stage.renderSurfaceID, stage.stateData.depthStencilState, stage.stateData.blendState, stage.stateData.viewportWidth, stage.stateData.viewportHeight, stage.stateData.cullState);
			};

			for(const auto i : m_ExecutionOrder)
			{
				if(i.stageType == Stage::Type::Compute)
					InitializeComputeStage(m_ComputeStages[i.stageIndex]);
				else
					InitializeRenderStage(m_RenderStages[i.stageIndex]);
			}
		}
	
		void ExecutableGraph::Execute(FrameContext &frameContext)
		{
			auto ExecuteComputeStage = [&](ExecutableComputeStage &stage)
			{
				//Cmd buffer is on compute queue so that the submission is batched with the stage's submissions during execute()
				RHI::CommandBufferID commandBufferID = frameContext.GetComputeQueue().AvailableCommandBuffer();
				RHI::CommandBuffer *commandBuffer = &g_Device->GetResources().commandBuffers.Get(commandBufferID);

				RHI::TransferCommandRecorder commandRecorder(commandBuffer);
				for(const auto &i : stage.textureStateUpdates)
				{
					GM_LOG_CORE_INFO("Updating state for texture %s", i.first.c_str());
					commandRecorder.UpdateState(m_ResourceManager.GetTexture(i.first), i.second);
				}

				commandRecorder.EndRecording();
				frameContext.GetComputeQueue().Submit(commandBufferID);
				//MAKE EXECUTION FUNCTION FOR COMPUTE TAKE A COMPUTESTATE, TEMPORARY NULLPTR BECAUSE CAN'T BE BOTHERED RN
				stage.execute(frameContext, m_ResourceManager, nullptr);
			};

			auto ExecuteRenderStage = [&](ExecutableRenderStage &stage)
			{
				//Cmd buffer is on render queue so that the submission is batched with the stage's submissions during execute()
				RHI::CommandBufferID commandBufferID = frameContext.GetRenderQueue().AvailableCommandBuffer();
				RHI::CommandBuffer *commandBuffer = &g_Device->GetResources().commandBuffers.Get(commandBufferID);

				RHI::TransferCommandRecorder commandRecorder(commandBuffer);
				for(const auto &i : stage.textureStateUpdates)
				{
					auto &texture = m_ResourceManager.GetTexture(i.first);
					if(texture.GetState() != i.second)
					{
						//GM_LOG_CORE_INFO("Updating state for texture %s", i.first.c_str());
						commandRecorder.UpdateState(texture, i.second);
					}
				}

				commandRecorder.EndRecording();
				frameContext.GetRenderQueue().Submit(commandBufferID);

				stage.execute(frameContext, m_ResourceManager, stage.state.get());
			};

			for(ExecutionData i : m_ExecutionOrder)
			{
				if(i.stageType == Stage::Type::Compute)
					ExecuteComputeStage(m_ComputeStages[i.stageIndex]);
				else
					ExecuteRenderStage(m_RenderStages[i.stageIndex]);
			}
		}
	}
}