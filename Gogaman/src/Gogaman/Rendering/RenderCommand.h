#pragma once

#include "RenderingContext.h"

namespace Gogaman
{
	namespace RenderCommand
	{
		struct RenderIndexed
		{
			static constexpr DispatchRenderCommandFunction DispatchFunction = &DispatchRenderCommand::RenderIndexed;

			//uint16_t            indexCount;
			//uint16_t            startIndex;
			//uint32_t            baseVertex;
			VertexArrayBufferID vertexArrayBuffer;
			IndexBufferID       indexBuffer;
		};

		struct DispatchComputeShader
		{
			static constexpr DispatchRenderCommandFunction DispatchFunction = &DispatchRenderCommand::DispatchComputeShader;

			uint16_t threadGroupCountX;
			uint16_t threadGroupCountY;
			uint16_t threadGroupCountZ;
		};

		struct BlendStateChange
		{
			static constexpr DispatchRenderCommandFunction DispatchFunction = &DispatchRenderCommand::BlendStateChange;

			BlendFactor sourceBlendFactor;
			BlendFactor destinationBlendFactor;
		};
	}

	using DispatchRenderCommandFunction = void (*)(const RenderingContext &, const void *);

	namespace DispatchRenderCommand
	{
		void RenderIndexed(const RenderingContext &context, const void *data)
		{
			const RenderCommand::RenderIndexed *renderCommand = static_cast<const RenderCommand::RenderIndexed *>(data);

			GetVertexArrayBuffer(renderCommand->vertexArrayBuffer).Bind();
			context.RenderIndexed(GetIndexBuffer(renderCommand->indexBuffer).GetIndexCount());
		}

		void DispatchComputeShader(const RenderingContext &context, const void *data)
		{
			const RenderCommand::DispatchComputeShader *renderCommand = static_cast<const RenderCommand::DispatchComputeShader *>(data);
			context.DispatchComputeShader(renderCommand->threadGroupCountX, renderCommand->threadGroupCountY, renderCommand->threadGroupCountZ);
		}

		void BlendStateChange(const RenderingContext &context, const void *data)
		{
			const RenderCommand::BlendStateChange *renderCommand = static_cast<const RenderCommand::BlendStateChange *>(data);
			context.SetBlendState(renderCommand->sourceBlendFactor, renderCommand->destinationBlendFactor);
		}
	}
}