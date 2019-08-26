#pragma once

#include "Gogaman/Core/CRTP.h"

#include "Shader/Shader.h"
#include "VertexArrayBuffer.h"
#include "IndexBuffer.h"
#include "RenderState.h"

namespace Gogaman
{
	enum class RenderCommandType : uint8_t
	{
		RenderIndexed,
		RenderInstanced,
		Compute,
		BlendStateChange
	};

	using RenderCommandKey = uint32_t;

	template<typename ImplementationType>
	struct AbstractRenderCommand : public CRTP<ImplementationType, AbstractRenderCommand>
	{
		static constexpr RenderCommandType GetTypeStatic() { return ImplementationType::GetTypeStatic(); }
		inline           RenderCommandType GetType() const { return ImplementationType::GetTypeStatic(); }

		RenderCommandKey key;
	};

	struct RenderIndexedCommand : public AbstractRenderCommand<RenderIndexedCommand>
	{
		static constexpr RenderCommandType GetTypeStatic() { return RenderCommandType::RenderIndexed; }

		VertexArrayBufferID vertexArrayBuffer;
		IndexBufferID       indexBuffer;
	};

	struct RenderInstancedCommand : public AbstractRenderCommand<RenderInstancedCommand>
	{
		static constexpr RenderCommandType GetTypeStatic() { return RenderCommandType::RenderInstanced; }
	};

	struct ComputeCommand : public AbstractRenderCommand<ComputeCommand>
	{
		static constexpr RenderCommandType GetTypeStatic() { return RenderCommandType::Compute; }

		ShaderID computeShader;
	};

	struct BlendStateChangeCommand : public AbstractRenderCommand<BlendStateChangeCommand>
	{
		static constexpr RenderCommandType GetTypeStatic() { return RenderCommandType::BlendStateChange; }

		BlendState state;
	};
}