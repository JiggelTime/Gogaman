#include "pch.h"
#include "OpenGL_Shader.h"

#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	Shader::Shader()
	{
		m_RendererID = glCreateProgram();
		GM_LOG_CORE_TRACE("Created shader with ID: %d", m_RendererID);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_RendererID);
	}

	void Shader::Compile(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const std::string &geometryShaderSource)
	{
		bool geometryShaderPresent = (geometryShaderSource.empty()) ? false : true;

		GLuint vertexShader, fragmentShader, geometryShader;

		//Compile vertex shader
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		const char *vertexShaderSourceData = vertexShaderSource.c_str();
		glShaderSource(vertexShader, 1, &vertexShaderSourceData, nullptr);
		glCompileShader(vertexShader);
		ValidateShader(vertexShader, "vertex shader");

		//Compile fragment shader
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		const char *fragmentShaderSourceData = fragmentShaderSource.c_str();
		glShaderSource(fragmentShader, 1, &fragmentShaderSourceData, nullptr);
		glCompileShader(fragmentShader);
		ValidateShader(fragmentShader, "fragment shader");

		//Compile geometry shader if present
		if(geometryShaderPresent)
		{
			geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
			const char *geometryShaderSourceData = geometryShaderSource.c_str();
			glShaderSource(geometryShader, 1, &geometryShaderSourceData, nullptr);
			glCompileShader(geometryShader);
			ValidateShader(geometryShader, "geometry shader");
		}
		
		//Compile shader program
		glAttachShader(m_RendererID, vertexShader);
		glAttachShader(m_RendererID, fragmentShader);
		if(geometryShaderPresent)
			glAttachShader(m_RendererID, geometryShader);
		glLinkProgram(m_RendererID);
		ValidateShader(m_RendererID, "shader program");

		glDetachShader(m_RendererID, vertexShader);
		glDetachShader(m_RendererID, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		
		if(geometryShaderPresent)
		{
			glDetachShader(m_RendererID, geometryShader);
			glDeleteShader(geometryShader);
		}
	}

	void Shader::Compile(const std::string &computeShaderSource)
	{
		GLuint computeShader;

		//Compile compute shader
		computeShader = glCreateShader(GL_COMPUTE_SHADER);
		const char *computeShaderSourceData = computeShaderSource.c_str();
		glShaderSource(computeShader, 1, &computeShaderSourceData, nullptr);
		glCompileShader(computeShader);
		ValidateShader(computeShader, "compute shader");

		//Compile shader program
		m_RendererID = glCreateProgram();
		glAttachShader(m_RendererID, computeShader);
		glLinkProgram(m_RendererID);
		ValidateShader(m_RendererID, "shader program");

		glDetachShader(m_RendererID, computeShader);
		glDeleteShader(computeShader);
	}

	//Supposed to be no-op for release builds
	void Shader::ValidateShader(const uint32_t object, const std::string &type)
	{
		int success;
		if(type != "shader program")
		{
			glGetShaderiv(object, GL_COMPILE_STATUS, &success);
			if(!success)
			{
				GLint logSize = 0;
				glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logSize);
				GLchar *infoLog = new GLchar[logSize];
				glGetShaderInfoLog(object, logSize, nullptr, infoLog);
				GM_LOG_CORE_ERROR("Failed to compile %s | Error: %s", type, infoLog);
			}
		}
		else
		{
			glGetProgramiv(object, GL_LINK_STATUS, &success);
			if(!success)
			{
				GLint logSize = 0;
				glGetProgramiv(object, GL_INFO_LOG_LENGTH, &logSize);
				GLchar *infoLog = new GLchar[logSize];
				glGetProgramInfoLog(object, logSize, nullptr, infoLog);
				GM_LOG_CORE_ERROR("Failed to link %s | Error: %s", type, infoLog);
			}
		}
	}

	GLint Shader::GetUniformLocation(const std::string &name)
	{
		auto iterator = m_UniformLocations.find(name);
		//Location not in location cache
		if(iterator == m_UniformLocations.end())
		{
			GLint location = glGetUniformLocation(m_RendererID, name.c_str());
			GM_ASSERT((location != -1) || (name == "debug") || (name == "debug2"), "Failed to get uniform location: invalid name")
			m_UniformLocations[name] = location;
			return location;
		}
		//Location already in location cache
		else
			return iterator->second;
	}
}