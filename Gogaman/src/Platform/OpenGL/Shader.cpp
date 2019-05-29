#include "pch.h"
#include "Shader.h"
#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	Shader::Shader()
		: m_ID(0)
	{}

	Shader::~Shader()
	{
		if(!m_ID)
			return;

		glDeleteProgram(m_ID);
	}

	void Shader::Compile(const GLchar *vertexShaderSource, const GLchar *fragmentShaderSource, const GLchar *geometryShaderSource)
	{
		bool geometryShaderPresent = (geometryShaderSource == nullptr) ? false : true;

		GLuint vertexShader, fragmentShader, geometryShader;

		//Compile vertex shader
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
		glCompileShader(vertexShader);
		CheckCompileErrors(vertexShader, "vertex shader");

		//Compile fragment shader
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
		glCompileShader(fragmentShader);
		CheckCompileErrors(fragmentShader, "fragment shader");

		//Compile geometry shader if present
		if(geometryShaderPresent)
		{
			geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometryShader, 1, &geometryShaderSource, nullptr);
			glCompileShader(geometryShader);
			CheckCompileErrors(geometryShader, "geometry shader");
		}

		//Compile shader program
		m_ID = glCreateProgram();
		glAttachShader(m_ID, vertexShader);
		glAttachShader(m_ID, fragmentShader);
		if(geometryShaderPresent)
			glAttachShader(m_ID, geometryShader);
		glLinkProgram(m_ID);
		CheckCompileErrors(m_ID, "shader program");

		//Delete shaders
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		if(geometryShaderPresent)
			glDeleteShader(geometryShader);
	}

	void Shader::Compile(const GLchar *computeShaderSource)
	{
		GLuint computeShader;

		//Compile compute shader
		computeShader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(computeShader, 1, &computeShaderSource, nullptr);
		glCompileShader(computeShader);
		CheckCompileErrors(computeShader, "compute shader");

		//Compile shader program
		m_ID = glCreateProgram();
		glAttachShader(m_ID, computeShader);
		glLinkProgram(m_ID);
		CheckCompileErrors(m_ID, "shader program");

		glDeleteShader(computeShader);
	}

	void Shader::CheckCompileErrors(const GLuint object, const std::string &type)
	{
		int success;
		if(type != "shader program")
		{
			glGetShaderiv(object, GL_COMPILE_STATUS, &success);
			if(!success)
			{
				GLint logSize = 0;
				glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logSize);
				GLchar* infoLog = new GLchar[logSize];
				glGetShaderInfoLog(object, logSize, nullptr, infoLog);
				GM_LOG_CORE_ERROR("Failed to compile %s", type);
				GM_LOG_CORE_ERROR("%s", infoLog);
			}
		}
		else
		{
			glGetProgramiv(object, GL_LINK_STATUS, &success);
			if(!success)
			{
				GLint logSize = 0;
				glGetProgramiv(object, GL_INFO_LOG_LENGTH, &logSize);
				GLchar* infoLog = new GLchar[logSize];
				glGetProgramInfoLog(object, logSize, nullptr, infoLog);
				GM_LOG_CORE_ERROR("Failed to link %s", type);
				GM_LOG_CORE_ERROR("%s", infoLog);
			}
		}
	}
}