#pragma once

#include "Shader.h"

namespace Gogaman
{
	class ShaderLoader
	{
	public:
		ShaderLoader();
		~ShaderLoader();

		std::unique_ptr<Shader> Load(const char *vertexShaderPath, const char *fragmentShaderPath, const char *geometryShaderPath = nullptr);
		std::unique_ptr<Shader> Load(const char *computeShaderPath);
	};
}