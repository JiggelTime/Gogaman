#pragma once

#include "RenderingContext.h"

namespace Gogaman
{
	struct PBR_Material
	{
		Texture2D_ID albedo;
		Texture2D_ID normal;
		Texture2D_ID roughness;
		Texture2D_ID metalness;
		Texture2D_ID emissivity;
	};
}