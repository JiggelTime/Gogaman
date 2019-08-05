#pragma once

#include "RenderCommand.h"

#define GM_MAX_RENDER_COMMANDS 1024

namespace Gogaman
{
	class RenderingContext;

	class Renderer
	{
	public:
		//void AddRenderCommand(RenderCommand &&command);

		void SortRenderCommands();

		void DispatchRenderCommands(RenderingContext &context);
	private:
		uint32_t                                          m_NumRenderCommands;
		std::array<uint32_t,      GM_MAX_RENDER_COMMANDS> m_RenderCommandIndices;
		//std::array<RenderCommand, GM_MAX_RENDER_COMMANDS> m_RenderCommands;
	};
}