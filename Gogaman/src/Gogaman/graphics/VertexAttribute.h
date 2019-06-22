#pragma once

#include "Shader.h"

namespace Gogaman
{
	struct VertexAttribute
	{
		VertexAttribute()
			: dataType(ShaderDataType::None), offset(0), normalized(false)
		{}

		VertexAttribute(const ShaderDataType dataType, const bool normalized = false)
			: dataType(dataType), offset(0), normalized(normalized)
		{}

		ShaderDataType dataType;
		uint8_t        offset;
		bool           normalized;
	};
}