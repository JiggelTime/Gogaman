#pragma once

#include "Gogaman/Base.h"
#if GM_RENDERING_API == GM_RENDERING_API_OPENGL
	#include "Platform/OpenGL/OpenGL_RenderCommand.h"
#else
	#error
#endif