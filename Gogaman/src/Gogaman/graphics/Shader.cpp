#include "pch.h"
#include "Shader.h"
#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	Shader::Shader(const char* computePath)
	{
		//Retrieve shader source code from filepath
		std::string computeCode;
		std::ifstream cShaderFile;

		//Make sure ifstream objects can throw exceptions
		cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			//Open file
			cShaderFile.open(computePath);
			std::stringstream cShaderStream;
			//Read buffer contents into streams
			cShaderStream << cShaderFile.rdbuf();
			//Close file handler
			cShaderFile.close();
			//Convert stream into string
			computeCode = cShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			//GM_LOG_CORE_ERROR("Unable to read shader file");
		}
		const char* cShaderCode = computeCode.c_str();

		unsigned int compute;

		//Compile compute shader
		compute = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(compute, 1, &cShaderCode, NULL);
		glCompileShader(compute);
		checkCompileErrors(compute, "compute shader");

		//Compile shader program
		id = glCreateProgram();
		glAttachShader(id, compute);
		glLinkProgram(id);
		checkCompileErrors(id, "shader program");

		//Delete shaders
		glDeleteShader(compute);
	}

	Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
	{
		//Retrieve shader source code from filepath
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream gShaderFile;

		//Make sure ifstream objects can throw exceptions
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			//Open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;

			//Read buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			//Close file handlers
			vShaderFile.close();
			fShaderFile.close();

			//Convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();

			//Load geometry shader if present
			if (geometryPath != nullptr)
			{
				gShaderFile.open(geometryPath);
				std::stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
			}
		}
		catch (std::ifstream::failure e)
		{
			//GM_LOG_CORE_ERROR("Unable to read shader file");
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		unsigned int vertex, fragment;

		//Compile vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "vertex shader");

		//Compile fragment shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "fragment shader");

		//Compile geometry shader
		unsigned int geometry;
		if (geometryPath != nullptr)
		{
			const char* gShaderCode = geometryCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			checkCompileErrors(geometry, "geometry shader");
		}

		//Compile shader program
		id = glCreateProgram();
		glAttachShader(id, vertex);
		glAttachShader(id, fragment);
		if (geometryPath != nullptr)
			glAttachShader(id, geometry);
		glLinkProgram(id);
		checkCompileErrors(id, "shader program");

		//Delete shaders
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometryPath != nullptr)
			glDeleteShader(geometry);
	}

	Shader::~Shader()
	{
	}

	void Shader::checkCompileErrors(unsigned int &shader, std::string type)
	{
		int success;
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				GLint logSize = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
				GLchar* infoLog = new GLchar[logSize];
				glGetShaderInfoLog(shader, logSize, nullptr, infoLog);
				//GM_LOG_CORE_ERROR("Unable to compile %s", type);
				//GM_LOG_CORE_ERROR("%s", infoLog);
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				GLint logSize = 0;
				glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &logSize);
				GLchar* infoLog = new GLchar[logSize];
				glGetProgramInfoLog(shader, logSize, nullptr, infoLog);
				//GM_LOG_CORE_ERROR("Unable to link %s", type);
				//GM_LOG_CORE_ERROR("%s", infoLog);
			}
		}
	}
}