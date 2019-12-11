#pragma once

#include "Gogaman/Core/CRTP.h"

#include "Identifier.h"

#include "CommandBuffer.h"

namespace Gogaman
{
	namespace RHI
	{
		class CommandBuffer;

		class ComputeState;

		template<typename ImplementationType>
		class AbstractComputeCommandRecorder : public CRTP<ImplementationType, AbstractComputeCommandRecorder>
		{
		public:
			AbstractComputeCommandRecorder(const AbstractComputeCommandRecorder &) = delete;
			AbstractComputeCommandRecorder(AbstractComputeCommandRecorder &&)      = delete;

			AbstractComputeCommandRecorder &operator=(const AbstractComputeCommandRecorder &) = delete;
			AbstractComputeCommandRecorder &operator=(AbstractComputeCommandRecorder &&)      = delete;

			inline void Dispatch(const uint32_t workGroupSizeXYZ[3]) const { this->GetImplementation().Dispatch(workGroupSizeXYZ); }

			inline void IndirectDispatch(const BufferID parameterBuffer, const uint64_t parameterBufferOffset = 0) const { this->GetImplementation().IndirectDispatch(parameterBuffer, parameterBufferOffset); }

			inline void StopRecording() { m_CommandBuffer->StopRecording(); }
		private:
			AbstractComputeCommandRecorder(CommandBuffer *commandBuffer, ComputeState *state)
				: m_CommandBuffer(commandBuffer), m_State(state)
			{
				m_CommandBuffer->StartRecording();
			}

			~AbstractComputeCommandRecorder() = default;
		protected:
			CommandBuffer *m_CommandBuffer;
			ComputeState  *m_State;
		private:
			friend ImplementationType;
		};
	}
}