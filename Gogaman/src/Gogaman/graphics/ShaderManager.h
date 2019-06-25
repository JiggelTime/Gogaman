#pragma once

#include "Shader.h"
#include "ShaderLoader.h"
#include "Gogaman/ResourceCache.h"

namespace Gogaman
{
	class ShaderManager
	{
	public:
		ShaderManager();
		~ShaderManager();

		ShaderID Create(const std::string &vertexShaderFilepath, const std::string &fragmentShaderFilepath, const std::string &geometryShaderFilepath = "");
		ShaderID Create(const std::string &computeShaderFilepath);

		void Reload(const ShaderID shaderID);
		void ReloadAll();

		const Shader &Get(const ShaderID shaderID) const;
	private:
		ShaderID                                                                        m_NextShaderID;
		ShaderLoader                                                                    m_Loader;

		ResourceCache<Shader, ShaderID>                                                 m_Shaders;
		std::unordered_map<ShaderID, std::tuple<std::string, std::string, std::string>> m_ShaderFilepaths;
		std::unordered_map<std::string, ShaderID>                                       m_FilepathShaders;
	};
}