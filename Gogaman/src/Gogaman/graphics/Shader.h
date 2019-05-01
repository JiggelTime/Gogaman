#pragma once

#include "Gogaman/Core.h"

#include <glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace Gogaman
{
	class GOGAMAN_API Shader
	{
	public:
		Shader();
		~Shader();

		void Compile(const GLchar *vertexShaderPath, const GLchar *fragmentShaderPath, const GLchar *geometryShaderPath = nullptr);
		void Compile(const GLchar *computerShaderPath);

		inline void Bind()
		{ glUseProgram(id); }

		inline void SetUniformBool(const std::string &name, bool value) const
		{ glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value); }

		inline void SetUniformInt(const std::string &name, int value) const
		{ glUniform1i(glGetUniformLocation(id, name.c_str()), value); }

		inline void SetUniformFloat(const std::string &name, float value) const
		{ glUniform1f(glGetUniformLocation(id, name.c_str()), value); }

		inline void SetUniformVec2(const std::string &name, const glm::vec2 &value) const
		{ glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]); }

		inline void SetUniformVec2(const std::string &name, float x, float y) const
		{ glUniform2f(glGetUniformLocation(id, name.c_str()), x, y); }

		inline void SetUniformVec3(const std::string &name, const glm::vec3 &value) const
		{ glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]); }

		inline void SetUniformVec3(const std::string &name, float x, float y, float z) const
		{ glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z); }

		inline void SetUniformVec4(const std::string &name, const glm::vec4 &value) const
		{ glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]); }

		inline void SetUniformVec4(const std::string &name, float x, float y, float z, float w)
		{ glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w); }

		inline void SetUniformMat2(const std::string &name, const glm::mat2 &Matrix) const
		{ glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &Matrix[0][0]); }

		inline void SetUniformMat3(const std::string &name, const glm::mat3 &Matrix) const
		{ glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &Matrix[0][0]); }

		inline void SetUniformMat4(const std::string &name, const glm::mat4 &Matrix) const
		{ glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &Matrix[0][0]); }
	private:
		void CheckCompileErrors(GLuint object, std::string type);
	public:
		unsigned int id;
	};
}