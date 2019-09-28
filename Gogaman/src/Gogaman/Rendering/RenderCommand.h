#pragma once

#include "Gogaman/Core/Base.h"
#include "Gogaman/Core/Logging/Log.h"

#include "RenderState.h"
#include "RenderingContext.h"

namespace Gogaman
{
	namespace RenderCommand
	{
		inline void RenderIndexed(RenderingContext &context, const void *data);
		inline void DispatchComputeShader(RenderingContext &context, const void *data);

		using DispatchFunction = void (*)(RenderingContext &, const void *);

		struct RenderIndexedCommand
		{
			static constexpr const DispatchFunction Dispatch = &RenderIndexed;

			//uint16_t            indexCount;
			//uint16_t            startIndex;
			//uint32_t            baseVertex;
			VertexArrayBufferID vertexArrayBuffer;
			IndexBufferID       indexBuffer;
		};

		struct DispatchComputeShaderCommand
		{
			static constexpr const DispatchFunction Dispatch = &DispatchComputeShader;

			uint16_t threadGroupCountX;
			uint16_t threadGroupCountY;
			uint16_t threadGroupCountZ;
		};
		
		GM_STATIC_ASSERT(std::is_pod<RenderIndexedCommand>::value,         "Render indexed command is not a POD type")
		GM_STATIC_ASSERT(std::is_pod<DispatchComputeShaderCommand>::value, "Dispatch compute shader command is not a POD type")

		inline void RenderIndexed(RenderingContext &context, const void *data)
		{
			const RenderCommand::RenderIndexedCommand *renderCommand = static_cast<const RenderCommand::RenderIndexedCommand *>(data);

			context.GetVertexArrayBuffers().Get(renderCommand->vertexArrayBuffer).Bind();
			context.RenderIndexed(context.GetIndexBuffers().Get(renderCommand->indexBuffer).GetIndexCount());
		}

		inline void DispatchComputeShader(RenderingContext &context, const void *data)
		{
			const RenderCommand::DispatchComputeShaderCommand *renderCommand = static_cast<const RenderCommand::DispatchComputeShaderCommand *>(data);
			context.DispatchComputeShader(renderCommand->threadGroupCountX, renderCommand->threadGroupCountY, renderCommand->threadGroupCountZ);
		}
	}
}