#pragma once

#include "Gogaman/Core/CRTP.h"

#include "TransferCommandRecorder.h"

namespace Gogaman
{
	namespace RHI
	{
		class CommandBuffer;

		class RenderState;

		class DescriptorGroup;

		class Buffer;

		template<typename ImplementationType>
		class AbstractRenderCommandRecorder : public CRTP<ImplementationType, AbstractRenderCommandRecorder>, public TransferCommandRecorder
		{
		public:
			AbstractRenderCommandRecorder(const AbstractRenderCommandRecorder &) = delete;
			AbstractRenderCommandRecorder(AbstractRenderCommandRecorder &&)      = delete;

			AbstractRenderCommandRecorder &operator=(const AbstractRenderCommandRecorder &) = delete;
			AbstractRenderCommandRecorder &operator=(AbstractRenderCommandRecorder &&)      = delete;

			inline void BindDescriptorGroup(const uint32_t bindingIndex, const DescriptorGroup &descriptorGroup) { this->GetImplementation().BindDescriptorGroup(bindingIndex, descriptorGroup); }

			inline void BindBuffer(const uint32_t bindingIndex, const Buffer &buffer) { this->GetImplementation().BindBuffer(bindingIndex, buffer); }

			inline void Render(const uint32_t vertexCount, const uint32_t startVertexOffset) { this->GetImplementation().Render(vertexCount, startVertexOffset); }
			
			inline void IndexedRender(const uint32_t indexCount, const uint32_t startIndexOffset, const uint32_t startVertexOffset) { this->GetImplementation().IndexedRender(indexCount, startIndexOffset, startVertexOffset); }

			inline void InstancedRender(const uint32_t vertexCount, const uint32_t startVertexOffset, const uint32_t instanceCount, const uint32_t startInstanceOffset) { this->GetImplementation().InstancedRender(vertexCount, startVertexOffset, instanceCount, startInstanceOffset); }
			
			inline void IndexedInstancedRender(const uint32_t indexCount, const uint32_t startIndexOffset, const uint32_t startVertexOffset, const uint32_t instanceCount, const uint32_t startInstanceOffset) { this->GetImplementation().IndexedInstancedRender(indexCount, startIndexOffset, startVertexOffset, instanceCount, startInstanceOffset); }

			inline void StopRecording() { this->GetImplementation().StopRecording(); }
		private:
			AbstractRenderCommandRecorder(CommandBuffer *commandBuffer, RenderState *state)
				: TransferCommandRecorder(commandBuffer), m_State(state)
			{}

			~AbstractRenderCommandRecorder() = default;
		protected:
			RenderState *m_State;
		private:
			friend ImplementationType;
		};
	}
}