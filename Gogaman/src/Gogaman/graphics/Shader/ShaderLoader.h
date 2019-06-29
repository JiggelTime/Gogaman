#pragma once

#include "Shader.h"

namespace Gogaman
{
	class ShaderLoader
	{
	public:
		ShaderLoader();
		~ShaderLoader();

		std::unique_ptr<Shader> Load(const char *vertexShaderFilepath, const char *fragmentShaderFilepath, const char *geometryShaderFilepath = nullptr);
		std::unique_ptr<Shader> Load(const char *computeShaderFilepath);
	};
}