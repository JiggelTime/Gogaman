#pragma once

#include "Gogaman/Core.h"
#include "Textures/Texture2D.h"
#include "Shader.h"
#include "Textures/Texture2D.h"
#include "Model.h"

#include <glad.h>

namespace Gogaman
{
	class GOGAMAN_API ResourceManager
	{
	public:
		static Shader LoadShader(std::string name, const GLchar *vertexShaderPath, const GLchar *fragmentShaderPath, const GLchar *geometryShaderPath = nullptr);
		static Shader LoadShader(std::string name, const GLchar *computeShaderPath);
		inline static Shader GetShader(std::string name) { return m_Shaders[name]; }

		static Texture2D LoadTexture2D(std::string name, const GLchar *filePath, GLboolean alpha);
		inline static Texture2D GetTexture2D(std::string name) { return m_Texture2Ds[name]; }

		static Model LoadModel(std::string name, const GLchar *filePath);
		inline static Model GetModel(std::string name) { return m_Models[name]; }

		static void Clear();
	private:
		ResourceManager();
		~ResourceManager();

		static void InitializeMaxAF();
		
		static Shader    LoadShaderFromFile(const GLchar *vertexShaderPath, const GLchar *fragmentShaderPath, const GLchar *geometryShaderPath = nullptr);
		static Shader    LoadShaderFromFile(const GLchar *computeShaderPath);
		static Texture2D LoadTexture2DFromFile(const GLchar *filePath, GLboolean sRGB);
		static Model     LoadModelFromFile(const GLchar *filePath);
	private:
		static std::map<std::string, Shader>    m_Shaders;
		static std::map<std::string, Texture2D> m_Texture2Ds;
		static std::map<std::string, Model>     m_Models;
		static float m_MaxAF;
	};
}