#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractComputeCommandRecorder.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		class ComputeState;

		class ComputeCommandRecorder : public AbstractComputeCommandRecorder<ComputeCommandRecorder>
		{
		public:
			ComputeCommandRecorder(CommandBuffer *commandBuffer, ComputeState *state);
			ComputeCommandRecorder(const ComputeCommandRecorder &) = delete;
			ComputeCommandRecorder(ComputeCommandRecorder &&)      = delete;

			~ComputeCommandRecorder() = default;

			ComputeCommandRecorder &operator=(const ComputeCommandRecorder &) = delete;
			ComputeCommandRecorder &operator=(ComputeCommandRecorder &&)      = delete;

			void Dispatch(const uint32_t workGroupSizeXYZ[3]) const;

			void IndirectDispatch(const BufferID parameterBuffer, const uint64_t parameterBufferOffset = 0) const;
		private:
			//friend AbstractComputeCommandRecorder;
		};
	}
}