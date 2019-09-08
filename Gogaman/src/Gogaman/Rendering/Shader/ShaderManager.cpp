#include "pch.h"
#include "ShaderManager.h"

#include "Gogaman/Core/Base.h"
#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	ShaderID ShaderManager::Create(const std::string &vertexShaderFilepath, const std::string &fragmentShaderFilepath, const std::string &geometryShaderFilepath)
	{
		GM_ASSERT(!vertexShaderFilepath.empty(),   "Failed to create shader: invalid vertex shader filepath");
		GM_ASSERT(!fragmentShaderFilepath.empty(), "Failed to create shader: invalid fragment shader filepath");
		
		auto iterator = m_FilepathShaders.find(vertexShaderFilepath);
		if(iterator == m_FilepathShaders.end())
		{
			std::unique_ptr<Shader> shader(ShaderLoader::Load(vertexShaderFilepath, fragmentShaderFilepath, geometryShaderFilepath));
			ShaderID shaderID = m_Shaders.Create(std::move(*shader.release()));
			m_FilepathShaders[vertexShaderFilepath] = shaderID;
			return shaderID;
		}
		else
			return iterator->second;
	}

	ShaderID ShaderManager::Create(const std::string &computeShaderFilepath)
	{
		GM_ASSERT(!computeShaderFilepath.empty(), "Failed to create compute shader: invalid filepath");
		
		auto iterator = m_FilepathShaders.find(computeShaderFilepath);
		if(iterator == m_FilepathShaders.end())
		{
			std::unique_ptr<Shader> shader(ShaderLoader::Load(computeShaderFilepath));
			ShaderID shaderID = m_Shaders.Create(std::move(*shader.release()));
			m_FilepathShaders[computeShaderFilepath] = shaderID;
			return shaderID;
		}
		else
			return iterator->second;
	}

	void ShaderManager::Reload(Shader &shader)
	{
		//TODO: Fix shader destructor not being called when overwritten
		std::string filepath1, filepath2, filepath3;
		std::tie(filepath1, filepath2, filepath3) = shader.GetFilepaths();
		if(filepath2.empty())
			shader = std::move(*ShaderLoader::Load(filepath1).release());
		else
			shader = std::move(*ShaderLoader::Load(filepath1, filepath2, filepath3).release());
	}

	void ShaderManager::ReloadAll()
	{
		for(auto &i : m_Shaders)
		{
			Reload(i);
		}
	}
}