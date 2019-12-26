#pragma once

#include "Gogaman/Core/CRTP.h"

#include "Identifier.h"

#include "TransferCommandRecorder.h"

namespace Gogaman
{
	namespace RHI
	{
		class CommandBuffer;

		class ComputeState;

		class DescriptorGroup;

		template<typename ImplementationType>
		class AbstractComputeCommandRecorder : public CRTP<ImplementationType, AbstractComputeCommandRecorder>, public TransferCommandRecorder
		{
		public:
			AbstractComputeCommandRecorder(const AbstractComputeCommandRecorder &) = delete;
			AbstractComputeCommandRecorder(AbstractComputeCommandRecorder &&)      = delete;

			AbstractComputeCommandRecorder &operator=(const AbstractComputeCommandRecorder &) = delete;
			AbstractComputeCommandRecorder &operator=(AbstractComputeCommandRecorder &&)      = delete;

			inline void BindDescriptorGroup(const uint32_t bindingIndex, const DescriptorGroup &descriptorGroup) { this->GetImplementation().BindDescriptorGroup(bindingIndex, descriptorGroup); }

			inline void Dispatch(const uint32_t workGroupSizeXYZ[3]) const { this->GetImplementation().Dispatch(workGroupSizeXYZ); }

			inline void IndirectDispatch(const BufferID parameterBuffer, const uint64_t parameterBufferOffset = 0) const { this->GetImplementation().IndirectDispatch(parameterBuffer, parameterBufferOffset); }
		private:
			AbstractComputeCommandRecorder(CommandBuffer *commandBuffer, ComputeState *state)
				: TransferCommandRecorder(commandBuffer), m_State(state)
			{}

			~AbstractComputeCommandRecorder() = default;
		protected:
			ComputeState *m_State;
		private:
			friend ImplementationType;
		};
	}
}