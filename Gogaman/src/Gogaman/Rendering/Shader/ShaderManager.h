#pragma once

#include "Shader.h"
#include "ShaderLoader.h"
#include "Gogaman/Resource.h"

namespace Gogaman
{
	//TODO: Use a fixed-size array to store shaders instead of a resource container (assume shaders cannot be created/deleted at runtime, only overwritten)
	//The number of shaders can be determined at compile-time using same method as for component type IDs.
	using ShaderID        = Resource::ID<uint8_t>;
	using ShaderContainer = Resource::Container<255, Shader, uint8_t>;

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