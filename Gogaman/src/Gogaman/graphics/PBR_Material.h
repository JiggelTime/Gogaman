#pragma once

#include "Shader.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace Gogaman
{
	struct PBR_Material
	{
		inline void SetShaderUniforms(Shader &shader) const
		{
			//Set uniforms here...
			shader.SetUniformVec3("materialAlbedo",      albedo);
			shader.SetUniformVec3("materialNormal",      normal);
			shader.SetUniformFloat("materialRoughness",  roughness);
			shader.SetUniformFloat("materialMetalness",  metalness);
			shader.SetUniformFloat("materialEmissivity", emissivity);
		}

		glm::vec3 albedo;
		glm::vec3 normal;
		float     roughness;
		float     metalness;
		float     emissivity;
	};
}