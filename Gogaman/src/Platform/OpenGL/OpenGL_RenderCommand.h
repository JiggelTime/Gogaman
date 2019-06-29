#pragma once

#include "Gogaman/Graphics/AbstractRenderCommand.h"

#include <glad.h>

namespace Gogaman
{
	class RenderCommand : AbstractRenderCommand<RenderCommand>
	{
		static inline void RenderIndexed(const uint32_t numIndices) { glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0); }
	};
}