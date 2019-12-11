#pragma once

#include "Gogaman/Core/CRTP.h"

#include "CommandBuffer.h"

namespace Gogaman
{
	namespace RHI
	{
		class CommandBuffer;

		class RenderState;

		template<typename ImplementationType>
		class AbstractRenderCommandRecorder : public CRTP<ImplementationType, AbstractRenderCommandRecorder>
		{
		public:
			AbstractRenderCommandRecorder(const AbstractRenderCommandRecorder &) = delete;
			AbstractRenderCommandRecorder(AbstractRenderCommandRecorder &&)      = delete;

			AbstractRenderCommandRecorder &operator=(const AbstractRenderCommandRecorder &) = delete;
			AbstractRenderCommandRecorder &operator=(AbstractRenderCommandRecorder &&)      = delete;

			inline void Render(const uint32_t vertexCount, const uint32_t startVertexOffset) { this->GetImplementation().Render(vertexCount, startVertexOffset); }
			
			inline void IndexedRender(const uint32_t indexCount, const uint32_t startIndexOffset, const uint32_t startVertexOffset) { this->GetImplementation().IndexedRender(indexCount, startIndexOffset, startVertexOffset); }

			inline void InstancedRender(const uint32_t vertexCount, const uint32_t startVertexOffset, const uint32_t instanceCount, const uint32_t startInstanceOffset) { this->GetImplementation().InstancedRender(vertexCount, startVertexOffset, instanceCount, startInstanceOffset); }
			
			inline void IndexedInstancedRender(const uint32_t indexCount, const uint32_t startIndexOffset, const uint32_t startVertexOffset, const uint32_t instanceCount, const uint32_t startInstanceOffset) { this->GetImplementation().IndexedInstancedRender(indexCount, startIndexOffset, startVertexOffset, instanceCount, startInstanceOffset); }

			inline void StopRecording() { m_CommandBuffer->StopRecording(); }
		private:
			AbstractRenderCommandRecorder(CommandBuffer *commandBuffer, RenderState *state)
				: m_CommandBuffer(commandBuffer), m_State(state)
			{
				m_CommandBuffer->StartRecording();
			}

			~AbstractRenderCommandRecorder() = default;
		protected:
			CommandBuffer *m_CommandBuffer;
			RenderState   *m_State;
		private:
			friend ImplementationType;
		};
	}
}