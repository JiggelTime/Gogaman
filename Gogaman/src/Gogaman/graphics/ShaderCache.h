#pragma once

#include "Shader.h"
#include "Gogaman/Base.h"
#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	class ShaderCache
	{
	public:
		inline void    Set(const ShaderID shaderID, std::unique_ptr<Shader> shader) { m_Shaders[shaderID] = std::move(shader); }
		inline Shader &Get(const ShaderID shaderID) { return *m_Shaders[shaderID].get(); }
	private:
		std::array<std::unique_ptr<Shader>, std::numeric_limits<ShaderID>::max()> m_Shaders;
	};
}