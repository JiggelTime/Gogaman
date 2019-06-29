#pragma once

#include "Gogaman/CRTP.h"

namespace Gogaman
{
	template<typename RenderCommandType>
	class AbstractRenderCommand : public CRTP<RenderCommandType, AbstractRenderCommand>
	{
	public:
		static inline void RenderIndexed(const uint32_t numIndices) { RenderCommandType::RenderIndexed(numIndices); }
	};
}