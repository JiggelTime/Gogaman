#pragma once

#include "Shader/Shader.h"

namespace Gogaman
{
	struct VertexAttribute
	{
		VertexAttribute(const ShaderDataType shaderDataType, const uint8_t offset = 0, const bool normalized = false)
			: dataType(shaderDataType), offset(offset), normalized(normalized)
		{}

		ShaderDataType dataType;
		uint8_t        offset;
		bool           normalized;
	};
}