#pragma once

#include "Base.h"
#include "Graphics/Shader.h"
#include "Platform/OpenGL/Texture2D.h"
#include "Graphics/Model.h"

#include <glad.h>

namespace Gogaman
{
	class ResourceManager
	{
	public:
		static void LoadShader(const std::string &name, const char *vertexShaderPath, const char *fragmentShaderPath, const char *geometryShaderPath = nullptr);
		static void LoadShader(const std::string &name, const char *computeShaderPath);
		//inline static Shader GetShader(const std::string &name) { return shaders[name]; }

		static void LoadTexture2D(const std::string &name, const char *filePath, const GLboolean sRGB);
		//inline static Texture2D GetTexture2D(const std::string &name) { return texture2Ds[name]; }

		static void LoadModel(const std::string &name, const char *filePath);
		inline static Model GetModel(const std::string &name) { return models[name]; }

		static void ReleaseAll();
	private:
		ResourceManager();
		~ResourceManager();
		
		static Shader    LoadShaderFromFile(const char *vertexShaderPath, const char *fragmentShaderPath, const char *geometryShaderPath = nullptr);
		static Shader    LoadShaderFromFile(const char *computeShaderPath);
		static Texture2D LoadTexture2DFromFile(const char *filePath, const GLboolean sRGB);
		static Model     LoadModelFromFile(const char *filePath);
	public:
		static std::unordered_map<std::string, Shader>    shaders;
		static std::unordered_map<std::string, Texture2D> texture2Ds;
		static std::unordered_map<std::string, Model>     models;
	};
}

//Macros
//#define GM_SHADER(x)    Gogaman::ResourceManager::GetShader(#x)
//#define GM_TEXTURE2D(x) Gogaman::ResourceManager::GetTexture2D(#x)
//#define GM_MODEL(x)     Gogaman::ResourceManager::GetModel(#x)

#define GM_SHADER(x)    Gogaman::ResourceManager::shaders[#x]
#define GM_TEXTURE2D(x) Gogaman::ResourceManager::texture2Ds[#x]
#define GM_MODEL(x)     Gogaman::ResourceManager::models[#x]