#pragma once

#include "Texture/Texture2D.h"

#include "Shader/Shader.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace Gogaman
{
	struct PBR_Material
	{
		std::unique_ptr<Texture2D> albedo;
		std::unique_ptr<Texture2D> normal;
		std::unique_ptr<Texture2D> roughness;
		std::unique_ptr<Texture2D> metalness;
		std::unique_ptr<Texture2D> emissivity;
	};
}