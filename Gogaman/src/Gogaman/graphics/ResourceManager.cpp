#include "pch.h"
#include "ResourceManager.h"
#include "Gogaman/Logging/Log.h"

#include <stb_image.h>

namespace Gogaman
{
	std::map<std::string, Shader> ResourceManager::m_Shaders;
	std::map<std::string, Texture2D> ResourceManager::m_Texture2Ds;
	std::map<std::string, Model> ResourceManager::m_Models;
	float ResourceManager::m_MaxAF;

	ResourceManager::ResourceManager()
	{
		InitializeMaxAF();
	}

	ResourceManager::~ResourceManager()
	{}

	void ResourceManager::InitializeMaxAF()
	{
		if(GL_ARB_texture_filter_anisotropic)
		{
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &m_MaxAF);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, m_MaxAF);
		}
		else
			m_MaxAF = 0.0f;

		GM_LOG_CORE_INFO("Max anisotropic texture filtering: x%d", m_MaxAF);
	}

	Shader ResourceManager::LoadShader(std::string name, const GLchar *vertexShaderPath, const GLchar *fragmentShaderPath, const GLchar *geometryShaderPath)
	{
		m_Shaders[name] = LoadShaderFromFile(vertexShaderPath, fragmentShaderPath, geometryShaderPath);
		return m_Shaders[name];
	}

	Shader ResourceManager::LoadShader(std::string name, const GLchar *computeShaderPath)
	{
		m_Shaders[name] = LoadShaderFromFile(computeShaderPath);
		return m_Shaders[name];
	}

	Texture2D ResourceManager::LoadTexture2D(std::string name, const GLchar *filePath, GLboolean alpha)
	{
		m_Texture2Ds[name] = LoadTexture2DFromFile(filePath, alpha);
		return m_Texture2Ds[name];
	}

	Model ResourceManager::LoadModel(std::string name, const GLchar *filePath)
	{
		m_Models[name] = LoadModelFromFile(filePath);
		return m_Models[name];
	}

	void ResourceManager::Clear()
	{
		for(auto i : m_Shaders)
			glDeleteProgram(i.second.id);

		for(auto i : m_Texture2Ds)
			glDeleteTextures(1, &i.second.id);
	}

	Shader ResourceManager::LoadShaderFromFile(const GLchar *vertexShaderPath, const GLchar *fragmentShaderPath, const GLchar *geometryShaderPath)
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
			GM_LOG_CORE_ERROR("Failed to read shader file at location %s", geometryShaderPath);
		}

		const char *vertexShaderSource   = vertexData.c_str();
		const char *fragmentShaderSource = fragmentData.c_str();
		const char *geometryShaderSource = geometryData.c_str();

		Shader shader;
		shader.Compile(vertexShaderSource, fragmentShaderSource, geometryShaderPresent ? geometryShaderSource : nullptr);
		return shader;
	}

	Shader ResourceManager::LoadShaderFromFile(const GLchar *computeShaderPath)
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

		GM_LOG_CORE_ERROR("Failed to read shader file at location %s", computeShaderPath);
		const char *source = data.c_str();
		
		Shader shader;
		shader.Compile(source);
		return shader;
	}

	Texture2D ResourceManager::LoadTexture2DFromFile(const GLchar *filePath, GLboolean sRGB)
	{
		Texture2D texture2D;
		int width, height, components;
		unsigned char *data = stbi_load(filePath, &width, &height, &components, 0);
		if(data)
		{
			if(components == 1)
			{
				texture2D.formatImage = GL_RED;
				texture2D.formatInternal = GL_RED;
			}
			else if(components == 3)
			{
				texture2D.formatImage = GL_RGB;
				texture2D.formatInternal = sRGB ? GL_SRGB : GL_RGB;
			}
			else if(components == 4)
			{
				texture2D.formatImage = GL_RGBA;
				texture2D.formatInternal = sRGB ? GL_SRGB_ALPHA : GL_RGBA;
			}
		}
		else
			GM_LOG_CORE_ERROR("Texture failed to load at location %s", filePath);
		
		texture2D.Generate(width, height, data);

		stbi_image_free(data);
		return texture2D;
	}

	Model ResourceManager::LoadModelFromFile(const GLchar *filePath)
	{
		std::string filePathString(filePath);
		Model model(filePathString);
		GM_LOG_CORE_INFO("Loaded model at: %s", filePath);
		return model;
	}
}