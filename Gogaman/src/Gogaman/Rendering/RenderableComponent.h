#pragma once

#include "Gogaman/RenderHardwareInterface/Identifier.h"

#include "Gogaman/Rendering/PBR_Material.h"

#include <glm.hpp>

namespace Gogaman
{
	struct RenderableComponent
	{
		glm::mat4 meshMatrix;
		glm::mat4 previousMeshMatrix;

		//uint32_t      materialIndex;

		RHI::BufferID vertexBufferID;
		RHI::BufferID indexBufferID;

		RHI::BufferID shaderDataBufferID;
	};
}