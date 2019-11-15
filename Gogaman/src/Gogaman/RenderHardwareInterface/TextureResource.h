#pragma once

#include "Gogaman/Core/ResourceContainer.h"

#include "Texture.h"

namespace Gogaman
{
	namespace RHI
	{
		using TextureContainer = ResourceContainer<Texture, uint16_t, 512>;
		using TextureID        = TextureContainer::ID;
	}
}