#include "pch.h"
#include "ResourceManager.h"
#include "Logging/Log.h"

#include <stb_image.h>

namespace Gogaman
{
	std::unordered_map<std::string, Shader> ResourceManager::shaders;
	std::unordered_map<std::string, Texture2D> ResourceManager::texture2Ds;
	std::unordered_map<std::string, Model> ResourceManager::models;

	ResourceManager::ResourceManager()
	{}

	ResourceManager::~ResourceManager()
	{}

	Shader ResourceManager::LoadShader(const std::string &name, const char *vertexShaderPath, const char *fragmentShaderPath, const char *geometryShaderPath)
	{
		if(shaders.find(name) == shaders.end())
			shaders[name] = LoadShaderFromFile(vertexShaderPath, fragmentShaderPath, geometryShaderPath);
		return shaders[name];
	}

	Shader ResourceManager::LoadShader(const std::string &name, const char *computeShaderPath)
	{
		if(shaders.find(name) == shaders.end())
			shaders[name] = LoadShaderFromFile(computeShaderPath);
		return shaders[name];
	}

	Texture2D ResourceManager::LoadTexture2D(const std::string &name, const char *filePath, const GLboolean &sRGB)
	{
		if(texture2Ds.find(name) == texture2Ds.end())
			texture2Ds[name] = LoadTexture2DFromFile(filePath, sRGB);
		return texture2Ds[name];
	}

	Model ResourceManager::LoadModel(const std::string &name, const char *filePath)
	{
		if(models.find(name) == models.end())
			models[name] = LoadModelFromFile(filePath);
		return models[name];
	}

	void ResourceManager::ReleaseAll()
	{
		for(auto i : shaders)
			i.second.Delete();

		for(auto i : texture2Ds)
			i.second.Delete();
	}

	Shader ResourceManager::LoadShaderFromFile(const char *vertexShaderPath, const char *fragmentShaderPath, const char *geometryShaderPath)
	{
		bool geometryShaderPresent = (geometryShaderPath == nullptr) ? false : true;

		std::string vertexData;
		std::string fragmentData;
		std::string geometryData;
		std::ifstream vertexInputStream;
		std::ifstream fragmentInputStream;
		std::ifstream geometryInputStream;
		vertexInputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragmentInputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		geometryInputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			//Open files
			vertexInputStream.open(vertexShaderPath);
			fragmentInputStream.open(fragmentShaderPath);
			std::stringstream vertexShaderStringStream, fragmentShaderStringStream;
			//Read buffer contents into streams
			vertexShaderStringStream << vertexInputStream.rdbuf();
			fragmentShaderStringStream << fragmentInputStream.rdbuf();
			//Close file handlers
			vertexInputStream.close();
			fragmentInputStream.close();

			vertexData = vertexShaderStringStream.str();
			fragmentData = fragmentShaderStringStream.str();

			//Load geometry shader if present
			if(geometryShaderPresent)
			{
				//Open file
				geometryInputStream.open(geometryShaderPath);
				std::stringstream geometryShaderStringStream;
				//Read buffer contents into stream
				geometryShaderStringStream << geometryInputStream.rdbuf();
				//Close file handler
				geometryInputStream.close();
				geometryData = geometryShaderStringStream.str();
			}
		}
		catch(std::ifstream::failure e)
		{
			GM_LOG_CORE_ERROR("Failed to read shader file at location %s", vertexShaderPath);
			GM_LOG_CORE_ERROR("Failed to read shader file at location %s", fragmentShaderPath);
			if(geometryShaderPresent)
				GM_LOG_CORE_ERROR("Failed to read shader file at location %s", geometryShaderPath);
		}

		const char *vertexShaderSource   = vertexData.c_str();
		const char *fragmentShaderSource = fragmentData.c_str();
		const char *geometryShaderSource = geometryData.c_str();

		Shader shader;
		shader.Compile(vertexShaderSource, fragmentShaderSource, geometryShaderPresent ? geometryShaderSource : nullptr);
		return shader;
	}

	Shader ResourceManager::LoadShaderFromFile(const char *computeShaderPath)
	{
		std::string data;
		std::ifstream inputStream;

		inputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			//Open file
			inputStream.open(computeShaderPath);
			std::stringstream stringstream;
			//Read buffer contents into stream
			stringstream << inputStream.rdbuf();
			//Close file handler
			inputStream.close();
			data = stringstream.str();
		}
		catch(std::ifstream::failure e)
		{
			GM_LOG_CORE_ERROR("Failed to read shader file at location %s", computeShaderPath);
		}

		const char *source = data.c_str();
		
		Shader shader;
		shader.Compile(source);
		return shader;
	}

	Texture2D ResourceManager::LoadTexture2DFromFile(const char *filePath, const GLboolean &sRGB)
	{
		Texture2D texture2D;
		int width, height, channels;
		unsigned char *data = stbi_load(filePath, &width, &height, &channels, 0);
		if(data)
		{
			if(channels == 1)
			{
				texture2D.formatImage = GL_RED;
				texture2D.formatInternal = GL_RED;
			}
			else if(channels == 3)
			{
				texture2D.formatImage = GL_RGB;
				texture2D.formatInternal = sRGB ? GL_SRGB : GL_RGB;
			}
			else if(channels == 4)
			{
				texture2D.formatImage = GL_RGBA;
				texture2D.formatInternal = sRGB ? GL_SRGB_ALPHA : GL_RGBA;
			}
		}
		else
			GM_LOG_CORE_ERROR("Texture failed to load at location %s", filePath);
		
		texture2D.Generate(width, height, data);
		GM_LOG_CORE_INFO("Loaded texture at location %s", filePath);
		stbi_image_free(data);
		return texture2D;
	}

	Model ResourceManager::LoadModelFromFile(const char *filePath)
	{
		std::string filePathString(filePath);
		Model model;
		model.LoadModel(filePathString);
		GM_LOG_CORE_INFO("Loaded model at location %s", filePath);
		return model;
	}
}