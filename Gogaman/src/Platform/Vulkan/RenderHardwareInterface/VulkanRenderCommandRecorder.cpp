#include "pch.h"
#include "VulkanRenderCommandRecorder.h"

#include "Gogaman/Core/Base.h"

#include "Gogaman/RenderHardwareInterface/RenderSurface.h"

#include "Gogaman/RenderHardwareInterface/RenderState.h"

#include "Gogaman/RenderHardwareInterface/DescriptorGroup.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	namespace RHI
	{
		RenderCommandRecorder::RenderCommandRecorder(CommandBuffer *commandBuffer, RenderState *state)
			: AbstractRenderCommandRecorder(commandBuffer, state)
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

			//TEMP CLEAR STUFF
			VkClearValue clearValues[2];
			clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
			clearValues[1].depthStencil.depth = 1.0f;
			clearValues[1].depthStencil.stencil = UINT32_MAX;
			renderPassBeginDescriptor.clearValueCount = 2;
			renderPassBeginDescriptor.pClearValues = clearValues;

			vkCmdBeginRenderPass(m_CommandBuffer->GetNativeData().vulkanCommandBuffer, &renderPassBeginDescriptor, VK_SUBPASS_CONTENTS_INLINE);
		}

		void RenderCommandRecorder::BindDescriptorGroup(const uint32_t bindingIndex, const DescriptorGroup &descriptorGroup)
		{
			vkCmdBindDescriptorSets(m_CommandBuffer->GetNativeData().vulkanCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_State->GetNativeData().vulkanPipelineLayout, bindingIndex, 1, &descriptorGroup.GetNativeData().vulkanDescriptorSet, 0, nullptr);
		}

		void RenderCommandRecorder::BindBuffer(const uint32_t bindingIndex, const Buffer &buffer)
		{
			/*
				Vertex          = 1 << 0,
				Index           = 1 << 1,
				Indirect        = 1 << 2,
				UnorderedAccess = 1 << 3,
				ShaderResource  = 1 << 4,
				ShaderConstants = 1 << 5
			*/
			VkDeviceSize offset = 0;

			switch(buffer.GetBindFlags())
			{
			case Buffer::BindFlag::Vertex:
				vkCmdBindVertexBuffers(m_CommandBuffer->GetNativeData().vulkanCommandBuffer, 0, 1, &buffer.GetNativeData().vulkanBuffer, &offset);
				break;
			case Buffer::BindFlag::Index:
				vkCmdBindIndexBuffer(m_CommandBuffer->GetNativeData().vulkanCommandBuffer, buffer.GetNativeData().vulkanBuffer, 0, VK_INDEX_TYPE_UINT16);
				break;
			default:
				GM_DEBUG_ASSERT(false, "Failed to bind buffer | Invalid buffer");
			}
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

		void RenderCommandRecorder::EndRecording()
		{
			vkCmdEndRenderPass(m_CommandBuffer->GetNativeData().vulkanCommandBuffer);

			m_CommandBuffer->EndRecording();
		}
	}
}