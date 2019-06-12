#pragma once

#include "Gogaman/ResourceCache.h"
#include "Shader.h"
#include "ShaderLoader.h"

namespace Gogaman
{
	class ShaderManager
	{
	public:
		ShaderManager();
		~ShaderManager();
	private:
		ResourceCache<Shader> m_Shaders;
	};
}