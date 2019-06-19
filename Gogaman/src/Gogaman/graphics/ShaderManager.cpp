#include "pch.h"
#include "ShaderManager.h"

#include "Gogaman/Base.h"
#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	ShaderManager::ShaderManager()
		: m_NextShaderID(0)
	{}

	ShaderManager::~ShaderManager()
	{}

	ShaderID ShaderManager::Create(const std::string &vertexShaderFilepath, const std::string &fragmentShaderFilepath, const std::string &geometryShaderFilepath)
	{
		GM_ASSERT(!vertexShaderFilepath.empty(),   "Failed to create shader: invalid vertex shader filepath");
		GM_ASSERT(!fragmentShaderFilepath.empty(), "Failed to create shader: invalid fragment shader filepath");
		GM_ASSERT(m_NextShaderID < (std::numeric_limits<ShaderID>::max() - 1), "Failed to create shader: maximum number of shaders (%d) reached", std::numeric_limits<ShaderID>::max() - 1);
		
		auto iterator = m_FilepathShaders.find(vertexShaderFilepath);
		if(iterator == m_FilepathShaders.end())
		{
			ShaderID shaderID = m_NextShaderID++;
			m_Shaders.Set(shaderID, m_Loader.Load(vertexShaderFilepath.c_str(), fragmentShaderFilepath.c_str(), geometryShaderFilepath.empty() ? nullptr : geometryShaderFilepath.c_str()));
			
			m_ShaderFilepaths[shaderID] = std::make_tuple(vertexShaderFilepath, fragmentShaderFilepath, geometryShaderFilepath);
			m_FilepathShaders[vertexShaderFilepath] = shaderID;
			return shaderID;
		}
		else
			return iterator->second;
	}

	ShaderID ShaderManager::Create(const std::string &computeShaderFilepath)
	{
		GM_ASSERT(!computeShaderFilepath.empty(), "Failed to create compute shader: invalid filepath");
		GM_ASSERT(m_NextShaderID < (std::numeric_limits<ShaderID>::max() - 1), "Failed to create shader: maximum number of shaders (%d) reached", std::numeric_limits<ShaderID>::max() - 1);
		
		auto iterator = m_FilepathShaders.find(computeShaderFilepath);
		if(iterator == m_FilepathShaders.end())
		{
			ShaderID shaderID = m_NextShaderID++;
			m_Shaders.Set(shaderID, m_Loader.Load(computeShaderFilepath.c_str()));

			m_ShaderFilepaths[shaderID] = std::make_tuple(computeShaderFilepath, "", "");
			m_FilepathShaders[computeShaderFilepath] = shaderID;
			return shaderID;
		}
		else
			return iterator->second;
	}

	void ShaderManager::Reload(const ShaderID shaderID)
	{
		GM_ASSERT(shaderID < m_NextShaderID, "Failed to reload shader: invalid shader ID");

		std::string filepath1, filepath2, filepath3;
		std::tie(filepath1, filepath2, filepath3) = m_ShaderFilepaths[shaderID];
		if(filepath2.empty())
			m_Shaders.Set(shaderID, m_Loader.Load(filepath1.c_str()));
		else
			m_Shaders.Set(shaderID, m_Loader.Load(filepath1.c_str(), filepath2.c_str(), filepath3.empty() ? nullptr : filepath3.c_str()));
	}

	void ShaderManager::ReloadAll()
	{
		for(ShaderID i = 0; i < (m_NextShaderID - 1); i++)
		{
			Reload(i);
		}
	}

	Shader &ShaderManager::Get(const ShaderID shaderID)
	{
		GM_ASSERT(shaderID < m_NextShaderID, "Failed to get shader: invalid ID");
		return m_Shaders.Get(shaderID);
	}
}