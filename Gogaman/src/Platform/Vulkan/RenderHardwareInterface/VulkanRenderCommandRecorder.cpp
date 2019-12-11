#include "pch.h"
#include "VulkanRenderCommandRecorder.h"

#include "Gogaman/RenderHardwareInterface/RenderState.h"

namespace Gogaman
{
	namespace RHI
	{
		RenderCommandRecorder::RenderCommandRecorder(CommandBuffer *commandBuffer, RenderState *state)
			:AbstractRenderCommandRecorder(commandBuffer, state)
		{
			vkCmdBindPipeline(m_CommandBuffer->GetNativeData().vulkanCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_State->GetNativeData().vulkanPipeline);
		}
		
		void RenderCommandRecorder::Render(const uint32_t vertexCount, const uint32_t startVertexOffset)
		{
			InstancedRender(vertexCount, startVertexOffset, 1, 0);
		}

		void RenderCommandRecorder::IndexedRender(const uint32_t indexCount, const uint32_t startIndexOffset, const uint32_t startVertexOffset)
		{
			IndexedInstancedRender(indexCount, startIndexOffset, startVertexOffset, 1, 0);
		}

		void RenderCommandRecorder::InstancedRender(const uint32_t vertexCount, const uint32_t startVertexOffset, const uint32_t instanceCount, const uint32_t startInstanceOffset)
		{
			vkCmdDraw(m_CommandBuffer->GetNativeData().vulkanCommandBuffer, vertexCount, instanceCount, startVertexOffset, startInstanceOffset);
		}

		void RenderCommandRecorder::IndexedInstancedRender(const uint32_t indexCount, const uint32_t startIndexOffset, const uint32_t startVertexOffset, const uint32_t instanceCount, const uint32_t startInstanceOffset)
		{
			vkCmdDrawIndexed(m_CommandBuffer->GetNativeData().vulkanCommandBuffer, indexCount, instanceCount, startIndexOffset, startVertexOffset, startInstanceOffset);
		}
	}
}