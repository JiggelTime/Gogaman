#include "pch.h"
#include "OpenGL_Shader.h"

#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	OpenGL_Shader::OpenGL_Shader()
	{}

	OpenGL_Shader::~OpenGL_Shader()
	{
		glDeleteProgram(m_ID);
	}

	Shader *Shader::Create()
	{
		return new OpenGL_Shader;
	}

	void OpenGL_Shader::Compile(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const std::string &geometryShaderSource)
	{
		bool geometryShaderPresent = (geometryShaderSource.empty()) ? false : true;

		GLuint vertexShader, fragmentShader, geometryShader;

		//Compile vertex shader
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		const char *vertexShaderSourceData = vertexShaderSource.c_str();
		glShaderSource(vertexShader, 1, &vertexShaderSourceData, nullptr);
		glCompileShader(vertexShader);
		CheckCompileErrors(vertexShader, "vertex shader");

		//Compile fragment shader
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		const char *fragmentShaderSourceData = fragmentShaderSource.c_str();
		glShaderSource(fragmentShader, 1, &fragmentShaderSourceData, nullptr);
		glCompileShader(fragmentShader);
		CheckCompileErrors(fragmentShader, "fragment shader");

		//Compile geometry shader if present
		if(geometryShaderPresent)
		{
			geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
			const char *geometryShaderSourceData = geometryShaderSource.c_str();
			glShaderSource(geometryShader, 1, &geometryShaderSourceData, nullptr);
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

		glDetachShader(m_ID, vertexShader);
		glDetachShader(m_ID, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		
		if(geometryShaderPresent)
		{
			glDetachShader(m_ID, geometryShader);
			glDeleteShader(geometryShader);
		}
	}

	void OpenGL_Shader::Compile(const std::string &computeShaderSource)
	{
		GLuint computeShader;

		//Compile compute shader
		computeShader = glCreateShader(GL_COMPUTE_SHADER);
		const char *computeShaderSourceData = computeShaderSource.c_str();
		glShaderSource(computeShader, 1, &computeShaderSourceData, nullptr);
		glCompileShader(computeShader);
		CheckCompileErrors(computeShader, "compute shader");

		//Compile shader program
		m_ID = glCreateProgram();
		glAttachShader(m_ID, computeShader);
		glLinkProgram(m_ID);
		CheckCompileErrors(m_ID, "shader program");

		glDetachShader(m_ID, computeShader);
		glDeleteShader(computeShader);
	}

	void OpenGL_Shader::CheckCompileErrors(const GLuint object, const std::string &type)
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