#ifndef SHADER_H
#define SHADER_H

#include <glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Shader {
public:
	unsigned int id;

	Shader(const char* computePath)
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
			std::cout << "Error reading shader file" << std::endl;
		}
		const char *cShaderCode = computeCode.c_str();

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
	
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
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
			if(geometryPath != nullptr)
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
			std::cout << "Error reading shader file" << std::endl;
		}
		const char *vShaderCode = vertexCode.c_str();
		const char *fShaderCode = fragmentCode.c_str();

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
			const char *gShaderCode = geometryCode.c_str();
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

	//Activate shader program
	void use()
	{
		glUseProgram(id);
	}

	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
	}

	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), value);
	}

	void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(id, name.c_str()), value);
	}

	void setVec2(const std::string &name, const glm::vec2 &value) const
	{
		glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}
	void setVec2(const std::string &name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
	}
	
	void setVec3(const std::string &name, const glm::vec3 &value) const
	{
		glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}
	void setVec3(const std::string &name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
	}
	
	void setVec4(const std::string &name, const glm::vec4 &value) const
	{
		glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}
	void setVec4(const std::string &name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
	}
	
	void setMat2(const std::string &name, const glm::mat2 &mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	
	void setMat3(const std::string &name, const glm::mat3 &mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

 	void setMat4(const std::string &name, const glm::mat4 &mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

private:
	//Check for compilation/linking errors
	void checkCompileErrors(unsigned int shader, std::string type) 
	{
		int success;
		if(type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if(!success)
			{
				GLint logSize = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
				GLchar *infoLog = new GLchar[logSize];
				glGetShaderInfoLog(shader, logSize, nullptr, infoLog);
				std::cout << "ERROR: Could not compile " << type << "\n" << infoLog << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if(!success)
			{
				GLint logSize = 0;
				glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &logSize);
				GLchar *infoLog = new GLchar[logSize];
				glGetProgramInfoLog(shader, logSize, nullptr, infoLog);
				std::cout << "ERROR: Could not link " << type << "\n" << infoLog << std::endl;
			}
		}
	}
};
#endif