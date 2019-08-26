#include "pch.h"
#include "Renderer.h"

#include "Gogaman/Core/Base.h"
#include "Gogaman/Logging/Log.h"

#include "RenderingContext.h"
#include "Gogaman/Sorting/RadixSort.h"

namespace Gogaman
{
	/*
	void Renderer::AddRenderCommand(RenderCommand &&command)
	{
		GM_ASSERT(m_NumRenderCommands < GM_MAX_RENDER_COMMANDS, "Failed to add render command: maximum number of render commands (%d) reached", GM_MAX_RENDER_COMMANDS)
		uint32_t index = m_NumRenderCommands++;
		m_RenderCommands[index]       = std::move(command);
		m_RenderCommandIndices[index] = index;
	}*/

	void Renderer::SortRenderCommands()
	{
		RadixSort::Sort(m_NumRenderCommands, m_RenderCommandIndices.data());
	}

	void Renderer::DispatchRenderCommands(RenderingContext &context)
	{
		/*
		for(const auto &i : m_RenderCommands)
		{
			m_RenderCommands[0].vertexArrayBuffer.Bind();
			context.RenderIndexed(m_RenderCommands[0].numIndices);
		}*/
	}
}