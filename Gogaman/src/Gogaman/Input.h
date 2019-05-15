#pragma once

#include "Core.h"

namespace Gogaman
{
	class GOGAMAN_API Input
	{
	public:
		inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressed(keycode); }
	protected:
		virtual bool IsKeyPressed(int keycode) = 0;
	private:
		static Input *s_Instance;
	};
}