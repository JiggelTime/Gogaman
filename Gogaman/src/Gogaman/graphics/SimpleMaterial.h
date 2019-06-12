#pragma once

#include "Shader.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace Gogaman
{
	struct SimpleMaterial
	{
		inline void SetShaderUniforms(Shader &shader) const
		{
			//Set uniforms here...
		}

		glm::vec3 diffuse;
		glm::vec3 normal;
		float     roughness;
		float     metallness;
	};
}