#pragma once

#include "Gogaman/Core.h"
#include "Shader.h"
#include "Textures/Texture2D.h"
#include "Textures/Texture3D.h"
#include "Model.h"

#include <glad.h>

namespace Gogaman
{
	class GOGAMAN_API ResourceManager
	{
	public:
		static Shader LoadShader(std::string name, const char *vertexShaderPath, const char *fragmentShaderPath, const char *geometryShaderPath = nullptr);
		static Shader LoadShader(std::string name, const char *computeShaderPath);
		inline static Shader GetShader(std::string name) { return m_Shaders[name]; }

		static Texture2D LoadTexture2D(std::string name, const char *filePath, GLboolean alpha);
		inline static Texture2D GetTexture2D(std::string name) { return m_Texture2Ds[name]; }

		static Model LoadModel(std::string name, const char *filePath);
		inline static Model GetModel(std::string name) { return m_Models[name]; }

		static void Clear();
	private:
		ResourceManager();
		~ResourceManager();

		static void InitializeMaxAF();
		
		static Shader    LoadShaderFromFile(const char *vertexShaderPath, const char *fragmentShaderPath, const char *geometryShaderPath = nullptr);
		static Shader    LoadShaderFromFile(const char *computeShaderPath);
		static Texture2D LoadTexture2DFromFile(const char *filePath, GLboolean sRGB);
		static Model     LoadModelFromFile(const char *filePath);
	private:
		static std::map<std::string, Shader>    m_Shaders;
		static std::map<std::string, Texture2D> m_Texture2Ds;
		static std::map<std::string, Texture3D> m_Texture3Ds;
		static std::map<std::string, Model>     m_Models;
		static float m_MaxAF;
	};
}

//Macros
#define GM_SHADER(x)    Gogaman::ResourceManager::GetShader("x")
#define GM_TEXTURE2D(x) Gogaman::ResourceManager::GetTexture2D("x")
#define GM_MODEL(x)     Gogaman::ResourceManager::GetModel("x")