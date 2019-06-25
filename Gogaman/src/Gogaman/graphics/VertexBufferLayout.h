#pragma once

#include "VertexAttribute.h"

#include "Shader.h"

namespace Gogaman
{
	class VertexBufferLayout
	{
	public:
		VertexBufferLayout()
		{}

		VertexBufferLayout(const std::initializer_list<VertexAttribute> &vertexAttributes)
			: m_Stride(0), m_VertexAttributes(vertexAttributes)
		{
			uint8_t offset = 0;
			for(auto &i : m_VertexAttributes)
			{
				i.offset              = offset;
				uint8_t attributeSize = Shader::GetShaderDataTypeSize(i.dataType);
				offset               += attributeSize;
				m_Stride             += attributeSize;
			}
		}

		std::vector<VertexAttribute>::iterator       begin()       { return m_VertexAttributes.begin(); }
		std::vector<VertexAttribute>::const_iterator begin() const { return m_VertexAttributes.begin(); }
		
		std::vector<VertexAttribute>::iterator       end()         { return m_VertexAttributes.end();   }
		std::vector<VertexAttribute>::const_iterator end()   const { return m_VertexAttributes.end();   }

		inline const uint8_t                       GetStride()           const { return m_Stride;           }
		inline const std::vector<VertexAttribute> &GetVertexAttributes() const { return m_VertexAttributes; }
	private:
		uint8_t                      m_Stride;
		std::vector<VertexAttribute> m_VertexAttributes;
	};
}