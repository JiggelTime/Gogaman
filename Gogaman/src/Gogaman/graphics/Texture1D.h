#pragma once

#include "Gogaman/Base.h"
#if GM_RENDERING_API == GM_RENDERING_API_OPENGL
	#include "Platform/OpenGL/OpenGL_Texture1D.h"
#else
	#error
#endif