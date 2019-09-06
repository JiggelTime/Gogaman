#pragma once

namespace Gogaman
{
	class Shader;

	class ShaderLoader
	{
	public:
		static std::unique_ptr<Shader> Load(const std::string &vertexShaderFilepath, const std::string &fragmentShaderFilepath, const std::string &geometryShaderFilepath);
		static std::unique_ptr<Shader> Load(const std::string &computeShaderFilepath);
	};
}