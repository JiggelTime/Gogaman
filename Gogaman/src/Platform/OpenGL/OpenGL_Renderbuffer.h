#pragma once

#include "Gogaman/Rendering/Texture/Texture.h"

#include <glad.h>

namespace Gogaman
{
	class Renderbuffer
	{
	public:
		Renderbuffer();
		Renderbuffer(const Renderbuffer &) = delete;
		Renderbuffer(Renderbuffer &&)      = default;

		~Renderbuffer();

		Renderbuffer &operator=(const Renderbuffer &) = delete;
		Renderbuffer &operator=(Renderbuffer &&)      = default;

		void Generate(const int width, const int height);

		inline uint32_t GetRendererID() const { return m_RendererID; }
	public:
		int width, height;
		TextureInternalFormat internalFormat;
	private:
		uint32_t m_RendererID;
	};
}