#pragma once

#include "Shader.h"
#include "ShaderLoader.h"
#include "Gogaman/Resource.h"

namespace Gogaman
{
	using ShaderID        = Resource::ID<uint32_t, 22, 10>;
	using ShaderContainer = Resource::Container<20, Shader, uint32_t, 22, 10>;

	class ShaderManager
	{
	public:
		ShaderManager()  = default;
		~ShaderManager() = default;

		ShaderID Create(const std::string &vertexShaderFilepath, const std::string &fragmentShaderFilepath, const std::string &geometryShaderFilepath = "");
		ShaderID Create(const std::string &computeShaderFilepath);

		inline void Reload(const ShaderID shaderID) { Reload(m_Shaders.Get(shaderID)); }
		void ReloadAll();

		inline constexpr Shader &Get(const ShaderID shaderID) { return m_Shaders.Get(shaderID); }
	private:
		void Reload(Shader &shader);
	private:
		ShaderLoader                              m_Loader;
		std::unordered_map<std::string, ShaderID> m_FilepathShaders;
		ShaderContainer                           m_Shaders;
	};
}