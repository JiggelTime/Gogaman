#pragma once

#include "Texture2D.h"

#include "Shader.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace Gogaman
{
	struct PBR_Material
	{
		inline void BindTextures() const
		{
			albedo->Bind(0);
			normal->Bind(1);
			roughness->Bind(2);
			metalness->Bind(3);
			emissivity->Bind(4);
		}

		std::unique_ptr<Texture2D> albedo;
		std::unique_ptr<Texture2D> normal;
		std::unique_ptr<Texture2D> roughness;
		std::unique_ptr<Texture2D> metalness;
		std::unique_ptr<Texture2D> emissivity;
	};
}