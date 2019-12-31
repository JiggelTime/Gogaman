#pragma once

#include "Shader.h"

namespace Gogaman
{
	namespace RHI
	{
		class VertexLayout
		{
		public:
			struct Attribute
			{
				Shader::DataType dataType;
				uint8_t          offset = 0;
			};
		public:
			VertexLayout(std::vector<Attribute> &&attributes)
				: m_Attributes(attributes)
			{
				m_Stride = 0;
				for(Attribute &i : m_Attributes)
				{
					i.offset = m_Stride;

					m_Stride += Shader::GetDataTypeSize(i.dataType);
				}
			}

			VertexLayout(const VertexLayout &) = delete;
			VertexLayout(VertexLayout &&)      = delete;

			~VertexLayout() = default;

			VertexLayout &operator=(const VertexLayout &) = delete;
			VertexLayout &operator=(VertexLayout &&)      = delete;

			inline constexpr const std::vector<Attribute> &GetAttributes() const { return m_Attributes; }

			inline constexpr uint8_t GetStride() const { return m_Stride; }
		private:
			std::vector<Attribute> m_Attributes;
			uint8_t                m_Stride;
		};
	}
}