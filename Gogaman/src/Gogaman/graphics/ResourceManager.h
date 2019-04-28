#pragma once

#include "Gogaman/Core.h"
#include "Textures/Texture2D.h"
#include "Shader.h"
#include "Textures/Texture2D.h"

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

		static void Clear();
	private:
		ResourceManager();
		~ResourceManager();

		static Shader LoadShaderFromFile(const GLchar *vertexShaderPath, const GLchar *fragmentShaderPath, const GLchar *geometryShaderPath = nullptr);
		static Shader LoadShaderFromFile(const GLchar *computeShaderPath);
		static Texture2D LoadTexture2DFromFile(const GLchar *filePath, GLboolean sRGB);
	private:
		static std::map<std::string, Shader> m_Shaders;
		static std::map<std::string, Texture2D> m_Texture2Ds;
	};
}