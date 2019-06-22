#pragma once

#include "Gogaman/Graphics/AbstractShader.h"

#include "Gogaman/Config.h"

#include <glad.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

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
			SetUniformBool("debug", GM_CONFIG.debug);
		}

		inline void SetUniformBool(const  std::string &name, const bool value)                                           const { glProgramUniform1i(m_RendererID,        glGetUniformLocation(m_RendererID, name.c_str()), (int)value); }
		inline void SetUniformInt(const   std::string &name, const int value)                                            const { glProgramUniform1i(m_RendererID,        glGetUniformLocation(m_RendererID, name.c_str()), value); }
		inline void SetUniformFloat(const std::string &name, const float value)                                          const { glProgramUniform1f(m_RendererID,        glGetUniformLocation(m_RendererID, name.c_str()), value); }
		inline void SetUniformVec2(const  std::string &name, const glm::vec2 &value)                                     const { glProgramUniform2fv(m_RendererID,       glGetUniformLocation(m_RendererID, name.c_str()), 1, &value[0]); }
		inline void SetUniformVec2(const  std::string &name, const float x, const float y)                               const { glProgramUniform2f(m_RendererID,        glGetUniformLocation(m_RendererID, name.c_str()), x, y); }
		inline void SetUniformVec3(const  std::string &name, const glm::vec3 &value)                                     const { glProgramUniform3fv(m_RendererID,       glGetUniformLocation(m_RendererID, name.c_str()), 1, &value[0]); }
		inline void SetUniformVec3(const  std::string &name, const float x, const float y, const float z)                const { glProgramUniform3f(m_RendererID,        glGetUniformLocation(m_RendererID, name.c_str()), x, y, z); }
		inline void SetUniformVec4(const  std::string &name, const glm::vec4 &value)                                     const { glProgramUniform4fv(m_RendererID,       glGetUniformLocation(m_RendererID, name.c_str()), 1, &value[0]); }
		inline void SetUniformVec4(const  std::string &name, const float x, const float y, const float z, const float w) const { glProgramUniform4f(m_RendererID,        glGetUniformLocation(m_RendererID, name.c_str()), x, y, z, w); }
		inline void SetUniformMat2(const  std::string &name, const glm::mat2 &Matrix)                                    const { glProgramUniformMatrix2fv(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, &Matrix[0][0]); }
		inline void SetUniformMat3(const  std::string &name, const glm::mat3 &Matrix)                                    const { glProgramUniformMatrix3fv(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, &Matrix[0][0]); }
		inline void SetUniformMat4(const  std::string &name, const glm::mat4 &Matrix)                                    const { glProgramUniformMatrix4fv(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, &Matrix[0][0]); }
	
		inline uint32_t GetRendererID() const { return m_RendererID; }

		static constexpr GLenum GetNativeShaderDataType(const ShaderDataType dataType)
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
			return 0;
		}
	private:
		void ValidateShader(const uint32_t object, const std::string &type);
	private:
		uint32_t m_RendererID;
	};
}