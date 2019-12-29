#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractTransferCommandRecorder.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		class TransferCommandRecorder : public AbstractTransferCommandRecorder<TransferCommandRecorder>
		{
		public:
			TransferCommandRecorder(CommandBuffer *commandBuffer);
			TransferCommandRecorder(const TransferCommandRecorder &) = delete;
			TransferCommandRecorder(TransferCommandRecorder &&)      = delete;

			~TransferCommandRecorder() = default;

			TransferCommandRecorder &operator=(const TransferCommandRecorder &) = delete;
			TransferCommandRecorder &operator=(TransferCommandRecorder &&)      = delete;

			void CopyData(Buffer &source, Buffer &destination);
			void CopyData(Buffer &source, Texture &destination);
		private:
			//friend AbstractTransferCommandRecorder;
		};
	}
}