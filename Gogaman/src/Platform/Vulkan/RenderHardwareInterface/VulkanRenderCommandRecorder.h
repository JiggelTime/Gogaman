#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractRenderCommandRecorder.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		class RenderState;

		class RenderCommandRecorder : public AbstractRenderCommandRecorder<RenderCommandRecorder>
		{
		public:
			RenderCommandRecorder(CommandBuffer *commandBuffer, RenderState *state);
			RenderCommandRecorder(const RenderCommandRecorder &) = delete;
			RenderCommandRecorder(RenderCommandRecorder &&)      = delete;

			~RenderCommandRecorder() = default;

			RenderCommandRecorder &operator=(const RenderCommandRecorder &) = delete;
			RenderCommandRecorder &operator=(RenderCommandRecorder &&)      = delete;

			void BindDescriptorGroup(const uint32_t bindingIndex, const DescriptorGroup &descriptorGroup);

			void BindBuffer(const uint32_t bindingIndex, const Buffer &buffer);

			void Render(const uint32_t vertexCount, const uint32_t startVertexOffset);

			void IndexedRender(const uint32_t indexCount, const uint32_t startIndexOffset, const uint32_t startVertexOffset);

			void InstancedRender(const uint32_t vertexCount, const uint32_t startVertexOffset, const uint32_t instanceCount, const uint32_t startInstanceOffset);

			void IndexedInstancedRender(const uint32_t indexCount, const uint32_t startIndexOffset, const uint32_t startVertexOffset, const uint32_t instanceCount, const uint32_t startInstanceOffset);
		
			void StopRecording();
		private:
			//friend AbstractRenderCommandRecorder;
		};
	}
}