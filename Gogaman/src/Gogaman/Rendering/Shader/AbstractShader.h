#pragma once

#include "Gogaman/Core/CRTP.h"

#include "Gogaman/Core/Base.h"
#include "Gogaman/Core/Logging/Log.h"

namespace Gogaman
{
	enum class ShaderDataType : uint8_t
	{
		Bool,
		Int,
		Int2,
		Int3,
		Int4,
		Float,
		Float2,
		Float3,
		Float4,
		Float2x2,
		Float3x3,
		Float4x4
	};

	template<typename ImplementationType>
	class AbstractShader : public CRTP<ImplementationType, AbstractShader>
	{
	public:
		AbstractShader(const AbstractShader &) = delete;
		AbstractShader(AbstractShader &&other) = default;

		AbstractShader &operator=(const AbstractShader &) = delete;
		AbstractShader &operator=(AbstractShader &&other) = default;

		inline void Compile(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const std::string &geometryShaderSource = "") { this->GetImplementation().Compile(vertexShaderSource, fragmentShaderSource, geometryShaderSource); }
		inline void Compile(const std::string &computeShaderSource) { this->GetImplementation().Compile(computeShaderSource); }

		inline void Bind() const { this->GetImplementation().Bind(); }

		inline void SetFilepaths(const std::tuple<std::string, std::string, std::string> &filepaths) { m_Filepaths = filepaths; }
		inline constexpr std::tuple<std::string, std::string, std::string> &GetFilepaths() { return m_Filepaths; }

		static inline constexpr uint8_t GetShaderDataTypeSize(const ShaderDataType dataType)
		{
			switch(dataType)
			{
			case ShaderDataType::Bool:
				return 1;
			case ShaderDataType::Int:
				return 4;
			case ShaderDataType::Int2:
				return 4 * 2;
			case ShaderDataType::Int3:
				return 4 * 3;
			case ShaderDataType::Int4:
				return 4 * 4;
			case ShaderDataType::Float:
				return 4;
			case ShaderDataType::Float2:
				return 4 * 2;
			case ShaderDataType::Float3:
				return 4 * 3;
			case ShaderDataType::Float4:
				return 4 * 4;
			case ShaderDataType::Float2x2:
				return 4 * 2 * 2;
			case ShaderDataType::Float3x3:
				return 4 * 3 * 3;
			case ShaderDataType::Float4x4:
				return 4 * 4 * 4;
			}

			GM_ASSERT(false, "Failed to get shader data type size | Invalid data type")
		}

		static inline constexpr ShaderDataType GetShaderDataBaseType(const ShaderDataType dataType)
		{
			switch(dataType)
			{
			case ShaderDataType::Bool:
				return ShaderDataType::Bool;
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
				return ShaderDataType::Int;
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			case ShaderDataType::Float2x2:
			case ShaderDataType::Float3x3:
			case ShaderDataType::Float4x4:
				return ShaderDataType::Float;
			}

			GM_ASSERT(false, "Failed to get shader data base type | Invalid data type")
		}

		static inline constexpr uint8_t GetShaderDataTypeComponentCount(const ShaderDataType dataType) { return GetShaderDataTypeSize(dataType) / GetShaderDataTypeSize(GetShaderDataBaseType(dataType)); }
	protected:
		inline AbstractShader()  = default;
		inline ~AbstractShader() = default;
	private:
		std::tuple<std::string, std::string, std::string> m_Filepaths;
	};
}