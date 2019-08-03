#pragma once

#include "Gogaman/CRTP.h"
#include "Gogaman/Resource.h"

#include "Gogaman/Base.h"
#include "Gogaman/Logging/Log.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace Gogaman
{
	enum class ShaderDataType : uint8_t
	{
		None = 0,
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

	using ShaderID = uint8_t;

	template<typename ShaderType>
	class AbstractShader : public CRTP<ShaderType, AbstractShader>, Resource<ShaderID>
	{
	public:
		AbstractShader(const AbstractShader &) = delete;
		AbstractShader(AbstractShader &&other) = default;

		AbstractShader &operator=(const AbstractShader &) = delete;
		AbstractShader &operator=(AbstractShader &&other) = default;

		inline void Compile(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const std::string &geometryShaderSource = "") { this->GetImplementation().Compile(vertexShaderSource, fragmentShaderSource, geometryShaderSource); }
		inline void Compile(const std::string &computeShaderSource)                                                                                       { this->GetImplementation().Compile(computeShaderSource); }

		inline void Bind() const { this->GetImplementation().Bind(); }

		inline void UploadUniform(const std::string &name, const bool       value) const { this->GetImplementation().UploadUniform(value); }
		inline void UploadUniform(const std::string &name, const int        value) const { this->GetImplementation().UploadUniform(value); }
		inline void UploadUniform(const std::string &name, const float      value) const { this->GetImplementation().UploadUniform(value); }
		inline void UploadUniform(const std::string &name, const glm::vec2 &value) const { this->GetImplementation().UploadUniform(value); }
		inline void UploadUniform(const std::string &name, const glm::vec3 &value) const { this->GetImplementation().UploadUniform(value); }
		inline void UploadUniform(const std::string &name, const glm::vec4 &value) const { this->GetImplementation().UploadUniform(value); }
		inline void UploadUniform(const std::string &name, const glm::mat2 &value) const { this->GetImplementation().UploadUniform(value); }
		inline void UploadUniform(const std::string &name, const glm::mat3 &value) const { this->GetImplementation().UploadUniform(value); }
		inline void UploadUniform(const std::string &name, const glm::mat4 &value) const { this->GetImplementation().UploadUniform(value); }

		static constexpr uint8_t GetShaderDataTypeSize(const ShaderDataType dataType)
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

			GM_ASSERT(false, "Failed to get shader data type size: invalid data type")
			return 0;
		}

		static constexpr ShaderDataType GetShaderDataBaseType(const ShaderDataType dataType)
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

			GM_ASSERT(false, "Failed to get shader data base type: invalid data type")
			return ShaderDataType::None;
		}

		static constexpr uint8_t GetNumShaderDataTypeComponents(const ShaderDataType dataType) { return GetShaderDataTypeSize(dataType) / GetShaderDataTypeSize(GetShaderDataBaseType(dataType)); }
	protected:
		AbstractShader()  = default;
		~AbstractShader() = default;
	};
}