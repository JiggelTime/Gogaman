#include "pch.h"
#include "ShaderLoader.h"

#include "Gogaman/Logging/Log.h"

#include "Shader.h"

namespace Gogaman
{
	std::unique_ptr<Shader> ShaderLoader::Load(const std::string &vertexShaderFilepath, const std::string &fragmentShaderFilepath, const std::string &geometryShaderFilepath)
	{
		bool geometryShaderPresent = geometryShaderFilepath.empty() ? false : true;

		std::string vertexShaderSource;
		std::string fragmentShaderSource;
		std::string geometryShaderSource;
		std::ifstream vertexInputStream;
		std::ifstream fragmentInputStream;
		std::ifstream geometryInputStream;
		vertexInputStream.exceptions(std::ifstream::failbit   | std::ifstream::badbit);
		fragmentInputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		geometryInputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			vertexInputStream.open(vertexShaderFilepath);
			fragmentInputStream.open(fragmentShaderFilepath);

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
				geometryInputStream.open(geometryShaderFilepath);

				std::stringstream geometryShaderStringStream;
				geometryShaderStringStream << geometryInputStream.rdbuf();

				geometryInputStream.close();

				geometryShaderSource = geometryShaderStringStream.str();
			}
		}
		catch(std::ifstream::failure e)
		{
			GM_LOG_CORE_ERROR("Failed to load shader file | Location: %s", vertexShaderFilepath);
			GM_LOG_CORE_ERROR("Failed to load shader file | Location: %s", fragmentShaderFilepath);
			if(geometryShaderPresent)
				GM_LOG_CORE_ERROR("Failed to load shader file | Location: %s", geometryShaderFilepath);
		}

		std::unique_ptr<Shader> shader(std::make_unique<Shader>());
		shader->SetFilepaths(std::make_tuple(vertexShaderFilepath, fragmentShaderFilepath, geometryShaderFilepath));
		if(geometryShaderPresent)
			shader->Compile(vertexShaderSource, fragmentShaderSource, geometryShaderSource);
		else
			shader->Compile(vertexShaderSource, fragmentShaderSource, "");
		return shader;
	}

	std::unique_ptr<Shader> ShaderLoader::Load(const std::string &computeShaderFilepath)
	{
		std::string   computeShaderSource;
		std::ifstream inputStream;

		inputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			inputStream.open(computeShaderFilepath);

			std::stringstream stringstream;
			stringstream << inputStream.rdbuf();

			inputStream.close();

			computeShaderSource = stringstream.str();
		}
		catch(std::ifstream::failure e)
		{
			GM_LOG_CORE_ERROR("Failed to load shader file | Location: %s", computeShaderFilepath);
		}

		std::unique_ptr<Shader> shader(std::make_unique<Shader>());
		shader->SetFilepaths(std::make_tuple(computeShaderFilepath, "", ""));
		shader->Compile(computeShaderSource);
		return shader;
	}
}