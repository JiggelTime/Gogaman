#pragma once

#include "Gogaman/Rendering/Texture/Texture.h"

#include <glad.h>

namespace Gogaman
{
	class RenderBuffer
	{
	public:
		RenderBuffer();
		RenderBuffer(const RenderBuffer &) = delete;
		RenderBuffer(RenderBuffer &&)      = default;

		~RenderBuffer();

		RenderBuffer &operator=(const RenderBuffer &) = delete;
		RenderBuffer &operator=(RenderBuffer &&)      = default;

		void Generate(const int width, const int height);

		inline uint32_t GetRendererID() const { return m_RendererID; }
	public:
		int width, height;
		TextureInternalFormat internalFormat;
	private:
		uint32_t m_RendererID;
	};
}