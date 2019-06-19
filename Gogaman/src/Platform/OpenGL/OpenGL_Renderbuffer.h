#pragma once

#include "Gogaman/Graphics/Texture.h"

#include <glad.h>

namespace Gogaman
{
	class Renderbuffer
	{
	public:
		Renderbuffer();
		Renderbuffer(const Renderbuffer &) = delete;
		Renderbuffer(Renderbuffer &&other) noexcept
			: m_RendererID(std::exchange(other.m_RendererID, 0)), width(std::exchange(other.width, 0)), height(std::exchange(other.height, 0)), internalFormat(std::exchange(other.internalFormat, TextureInternalFormat::None))
		{}

		~Renderbuffer();

		Renderbuffer &operator=(const Renderbuffer &) = delete;
		Renderbuffer &operator=(Renderbuffer &&other) noexcept
		{
			std::swap(m_RendererID,   other.m_RendererID);
			std::swap(width,          other.width);
			std::swap(height,         other.height);
			std::swap(internalFormat, other.internalFormat);
			return *this;
		}

		void Generate(const int width, const int height);

		inline uint32_t GetRendererID() const { return m_RendererID; }
	public:
		int width, height;
		TextureInternalFormat internalFormat;
	private:
		uint32_t m_RendererID;
	};
}