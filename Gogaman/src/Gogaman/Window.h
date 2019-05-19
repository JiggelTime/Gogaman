#pragma once

#include "Core.h"

namespace Gogaman
{
	class GOGAMAN_API Window
	{
	public:
		Window();

		virtual void Initialize() = 0;

		inline size_t GetWidth() const { return m_Width; }
		inline size_t GetHeight() const { return m_Height; }
	private:
		size_t m_Width;
		size_t m_Height
	};
}