#include "pch.h"
#include "VulkanRenderCommandRecorder.h"

#include "Gogaman/RenderHardwareInterface/RenderSurface.h"

#include "Gogaman/RenderHardwareInterface/RenderState.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	namespace RHI
	{
		RenderCommandRecorder::RenderCommandRecorder(CommandBuffer *commandBuffer, RenderState *state)
			:AbstractRenderCommandRecorder(commandBuffer, state)
		{
			vkCmdBindPipeline(m_CommandBuffer->GetNativeData().vulkanCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_State->GetNativeData().vulkanPipeline);

			const RenderSurface &renderSurface = g_Device->GetResources().renderSurfaces.Get(m_State->GetRenderSurfaceID());

			VkRenderPassBeginInfo renderPassBeginDescriptor = {};
			renderPassBeginDescriptor.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginDescriptor.renderPass               = renderSurface.GetNativeData().vulkanRenderPass;
			renderPassBeginDescriptor.framebuffer              = renderSurface.GetNativeData().vulkanFramebuffer;
			renderPassBeginDescriptor.renderArea.offset.x      = 0;
			renderPassBeginDescriptor.renderArea.offset.y      = 0;
			//TODO: Use state's viewport dimensions?
			renderPassBeginDescriptor.renderArea.extent.width  = renderSurface.GetWidth();
			renderPassBeginDescriptor.renderArea.extent.height = renderSurface.GetHeight();

			vkCmdBeginRenderPass(m_CommandBuffer->GetNativeData().vulkanCommandBuffer, &renderPassBeginDescriptor, VK_SUBPASS_CONTENTS_INLINE);
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

		void RenderCommandRecorder::StopRecording()
		{
			vkCmdEndRenderPass(m_CommandBuffer->GetNativeData().vulkanCommandBuffer);

			m_CommandBuffer->StopRecording();
		}
	}
}