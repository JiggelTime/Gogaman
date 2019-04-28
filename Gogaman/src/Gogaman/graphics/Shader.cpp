#include "pch.h"
#include "Shader.h"
#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	Shader::Shader()
	{}

	Shader::~Shader()
	{}

	void Shader::Compile(const GLchar *vertexShaderSource, const GLchar *fragmentShaderSource, const GLchar *geometryShaderSource)
	{
		bool geometryShaderPresent = (geometryShaderSource == nullptr) ? false : true;

		GLuint vertexShader, fragmentShader, geometryShader;

		//Compile vertex shader
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);
		CheckCompileErrors(vertexShader, "vertex shader");

		//Compile fragment shader
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);
		CheckCompileErrors(fragmentShader, "fragment shader");

		//Compile geometry shader if present
		if(geometryShaderPresent)
		{
			geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometryShader, 1, &geometryShaderSource, NULL);
			glCompileShader(geometryShader);
			CheckCompileErrors(geometryShader, "geometry shader");
		}

		//Compile shader program
		id = glCreateProgram();
		glAttachShader(id, vertexShader);
		glAttachShader(id, fragmentShader);
		if(geometryShaderPresent)
			glAttachShader(id, geometryShader);
		glLinkProgram(id);
		CheckCompileErrors(id, "shader program");

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
		glShaderSource(computeShader, 1, &computeShaderSource, NULL);
		glCompileShader(computeShader);
		CheckCompileErrors(computeShader, "compute shader");

		//Compile shader program
		id = glCreateProgram();
		glAttachShader(id, computeShader);
		glLinkProgram(id);
		CheckCompileErrors(id, "shader program");

		glDeleteShader(computeShader);
	}

	void Shader::CheckCompileErrors(GLuint object, std::string name)
	{
		int success;
		if(name != "shader program")
		{
			glGetShaderiv(object, GL_COMPILE_STATUS, &success);
			if(!success)
			{
				GLint logSize = 0;
				glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logSize);
				GLchar* infoLog = new GLchar[logSize];
				glGetShaderInfoLog(object, logSize, nullptr, infoLog);
				GM_LOG_CORE_ERROR("Unable to compile %s", name);
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
				GM_LOG_CORE_ERROR("Unable to link %s", name);
				GM_LOG_CORE_ERROR("%s", infoLog);
			}
		}
	}
}