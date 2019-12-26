#include "pch.h"
#include "VulkanTransferCommandRecorder.h"

namespace Gogaman
{
	namespace RHI
	{
		TransferCommandRecorder::TransferCommandRecorder(CommandBuffer *commandBuffer)
			: AbstractTransferCommandRecorder(commandBuffer)
		{}
	}
}