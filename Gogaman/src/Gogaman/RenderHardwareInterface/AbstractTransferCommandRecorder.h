#pragma once

#include "Gogaman/Core/CRTP.h"

#include "Gogaman/RenderHardwareInterface/Texture.h"

#include "CommandBuffer.h"

namespace Gogaman
{
	namespace RHI
	{
		class Buffer;

		template<typename ImplementationType>
		class AbstractTransferCommandRecorder : public CRTP<ImplementationType, AbstractTransferCommandRecorder>
		{
		public:
			AbstractTransferCommandRecorder(const AbstractTransferCommandRecorder &) = delete;
			AbstractTransferCommandRecorder(AbstractTransferCommandRecorder &&)      = delete;

			AbstractTransferCommandRecorder &operator=(const AbstractTransferCommandRecorder &) = delete;
			AbstractTransferCommandRecorder &operator=(AbstractTransferCommandRecorder &&)      = delete;

			inline void UpdateState(Texture &texture, const Texture::State state) { this->GetImplementation().UpdateState(texture, state); }

			inline void CopyData(Buffer &source, Buffer &destination) { this->GetImplementation().CopyData(source, destination); }
			inline void CopyData(Buffer &source, Texture &destination) { this->GetImplementation().CopyData(source, destination); }

			inline void EndRecording() { m_CommandBuffer->EndRecording(); }
		private:
			AbstractTransferCommandRecorder(CommandBuffer *commandBuffer)
				: m_CommandBuffer(commandBuffer)
			{
				m_CommandBuffer->StartRecording();
			}

			~AbstractTransferCommandRecorder() = default;
		protected:
			CommandBuffer *m_CommandBuffer;
		private:
			friend ImplementationType;
		};
	}
}