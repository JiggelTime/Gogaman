#include "pch.h"
#include "VulkanComputeCommandRecorder.h"

#include "Gogaman/Core/Base.h"

#include "Gogaman/RenderHardwareInterface/ComputeState.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	namespace RHI
	{
		ComputeCommandRecorder::ComputeCommandRecorder(CommandBuffer *commandBuffer, ComputeState *state)
			: AbstractComputeCommandRecorder(commandBuffer, state)
		{
			vkCmdBindPipeline(m_CommandBuffer->GetNativeData().vulkanCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_State->GetNativeData().vulkanPipeline);
		}

		void ComputeCommandRecorder::Dispatch(const uint32_t workGroupSizeXYZ[3]) const
		{
			vkCmdDispatch(m_CommandBuffer->GetNativeData().vulkanCommandBuffer, workGroupSizeXYZ[0], workGroupSizeXYZ[1], workGroupSizeXYZ[2]);
		}

		void ComputeCommandRecorder::IndirectDispatch(const BufferID parameterBuffer, const uint64_t parameterBufferOffset) const
		{
			//resource barrier here?
			vkCmdDispatchIndirect(m_CommandBuffer->GetNativeData().vulkanCommandBuffer, g_Device->GetResources().buffers.Get(parameterBuffer).GetNativeData().vulkanBuffer, parameterBufferOffset);
		}
	}
}