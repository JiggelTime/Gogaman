#include "pch.h"
#include "ShaderLoader.h"

#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	ShaderLoader::ShaderLoader()
	{}

	ShaderLoader::~ShaderLoader()
	{}

	std::unique_ptr<Shader> ShaderLoader::Load(const char *vertexShaderPath, const char *fragmentShaderPath, const char *geometryShaderPath)
	{
		bool geometryShaderPresent = (geometryShaderPath == nullptr) ? false : true;

		std::string vertexShaderSource;
		std::string fragmentShaderSource;
		std::string geometryShaderSource;
		std::ifstream vertexInputStream;
		std::ifstream fragmentInputStream;
		std::ifstream geometryInputStream;
		vertexInputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragmentInputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		geometryInputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			vertexInputStream.open(vertexShaderPath);
			fragmentInputStream.open(fragmentShaderPath);

			std::stringstream vertexShaderStringStream, fragmentShaderStringStream;
			vertexShaderStringStream   << vertexInputStream.rdbuf();
			fragmentShaderStringStream << fragmentInputStream.rdbuf();

			vertexInputStream.close();
			fragmentInputStream.close();

			vertexShaderSource   = vertexShaderStringStream.str();
			fragmentShaderSource = fragmentShaderStringStream.str();

			//Load geometry shader if present
			if(geometryShaderPresent)
			{
				geometryInputStream.open(geometryShaderPath);

				std::stringstream geometryShaderStringStream;
				geometryShaderStringStream << geometryInputStream.rdbuf();

				geometryInputStream.close();

				geometryShaderSource = geometryShaderStringStream.str();
			}
		}
		catch(std::ifstream::failure e)
		{
			GM_LOG_CORE_ERROR("Failed to load shader file | Location: %s", vertexShaderPath);
			GM_LOG_CORE_ERROR("Failed to load shader file | Location: %s", fragmentShaderPath);
			if(geometryShaderPresent)
				GM_LOG_CORE_ERROR("Failed to load shader file | Location: %s", geometryShaderPath);
		}

		std::unique_ptr<Shader> shader = std::unique_ptr<Shader>(Shader::Create());
		if(geometryShaderPresent)
			shader->Compile(vertexShaderSource, fragmentShaderSource, geometryShaderSource);
		else
			shader->Compile(vertexShaderSource, fragmentShaderSource);
		return shader;
	}

	std::unique_ptr<Shader> ShaderLoader::Load(const char *computeShaderPath)
	{
		std::string   computeShaderSource;
		std::ifstream inputStream;

		inputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			inputStream.open(computeShaderPath);

			std::stringstream stringstream;
			stringstream << inputStream.rdbuf();

			inputStream.close();

			computeShaderSource = stringstream.str();
		}
		catch(std::ifstream::failure e)
		{
			GM_LOG_CORE_ERROR("Failed to load shader file | Location: %s", computeShaderPath);
		}

		std::unique_ptr<Shader> shader = std::unique_ptr<Shader>(Shader::Create());
		shader->Compile(computeShaderSource);
		return shader;
	}
}