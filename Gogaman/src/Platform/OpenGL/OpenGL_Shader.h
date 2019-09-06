#pragma once

#include "Gogaman/Rendering/Shader/AbstractShader.h"

#include "Gogaman/Core/Base.h"
#include "Gogaman/Logging/Log.h"
#include "Gogaman/Core/Config.h"

#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include <glad.h>

namespace Gogaman
{
	class Shader : public AbstractShader<Shader>
	{
	public:
		Shader();
		Shader(const Shader &) = delete;
		Shader(Shader &&)      = default;

		~Shader();

		Shader &operator=(const Shader &) = delete;
		Shader &operator=(Shader &&)      = default;

		void Compile(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const std::string &geometryShaderSource = "");
		void Compile(const std::string &computerShaderSource);

		inline void Bind() const
		{
			glUseProgram(m_RendererID);

			#if GM_DEBUG_SHADER_UNIFORMS_ENABLED
				glProgramUniform1i(m_RendererID, glGetUniformLocation(m_RendererID, "debug"),  (int)GM_CONFIG.debug);
				glProgramUniform1i(m_RendererID, glGetUniformLocation(m_RendererID, "debug2"), (int)GM_CONFIG.debug2);
			#endif
		}

		inline void UploadUniform(const std::string &name, const bool       value) { glProgramUniform1i(m_RendererID,        GetUniformLocation(name.c_str()),                        (int)value);  }
		inline void UploadUniform(const std::string &name, const int        value) { glProgramUniform1i(m_RendererID,        GetUniformLocation(name.c_str()),                             value);  }
		inline void UploadUniform(const std::string &name, const float      value) { glProgramUniform1f(m_RendererID,        GetUniformLocation(name.c_str()),                             value);  }
		inline void UploadUniform(const std::string &name, const glm::vec2 &value) { glProgramUniform2fv(m_RendererID,       GetUniformLocation(name.c_str()), 1,           glm::value_ptr(value)); }
		inline void UploadUniform(const std::string &name, const glm::vec3 &value) { glProgramUniform3fv(m_RendererID,       GetUniformLocation(name.c_str()), 1,           glm::value_ptr(value)); }
		inline void UploadUniform(const std::string &name, const glm::vec4 &value) { glProgramUniform4fv(m_RendererID,       GetUniformLocation(name.c_str()), 1,           glm::value_ptr(value)); }
		inline void UploadUniform(const std::string &name, const glm::mat2 &value) { glProgramUniformMatrix2fv(m_RendererID, GetUniformLocation(name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); }
		inline void UploadUniform(const std::string &name, const glm::mat3 &value) { glProgramUniformMatrix3fv(m_RendererID, GetUniformLocation(name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); }
		inline void UploadUniform(const std::string &name, const glm::mat4 &value) { glProgramUniformMatrix4fv(m_RendererID, GetUniformLocation(name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); }
	
		GLint GetUniformLocation(const std::string &name);
		
		inline constexpr uint32_t GetRendererID() { return m_RendererID; }

		static inline constexpr GLenum GetNativeShaderDataType(const ShaderDataType dataType)
		{
			switch(dataType)
			{
			case ShaderDataType::Bool:
				return GL_BOOL;
			case ShaderDataType::Int:
				return GL_INT;
			case ShaderDataType::Int2:
				return GL_INT_VEC2;
			case ShaderDataType::Int3:
				return GL_INT_VEC3;
			case ShaderDataType::Int4:
				return GL_INT_VEC4;
			case ShaderDataType::Float:
				return GL_FLOAT;
			case ShaderDataType::Float2:
				return GL_FLOAT_VEC2;
			case ShaderDataType::Float3:
				return GL_FLOAT_VEC3;
			case ShaderDataType::Float4:
				return GL_FLOAT_VEC4;
			case ShaderDataType::Float2x2:
				return GL_FLOAT_MAT2;
			case ShaderDataType::Float3x3:
				return GL_FLOAT_MAT3;
			case ShaderDataType::Float4x4:
				return GL_FLOAT_MAT4;
			}

			GM_ASSERT(false, "Failed to get native shader data type: invalid data type")
		}
	private:
		void ValidateShader(const uint32_t object, const std::string &type);
	private:
		std::unordered_map<std::string, GLint> m_UniformLocations;

		uint32_t m_RendererID;
	};
}