#pragma once

#include "Gogaman/Core/Base.h"
#if GM_RENDERING_API == GM_RENDERING_API_OPENGL
	#include "Platform/OpenGL/OpenGL_Texture3D.h"
#else
	#error
#endif