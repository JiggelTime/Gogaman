#pragma once

#include "Gogaman/Base.h"
#include "Gogaman/Logging/Log.h"

#include "AbstractVertexBuffer.h"
#include "Gogaman/ResourceCache.h"

namespace Gogaman
{
	class VertexBufferManager
	{
	public:
		VertexBufferManager()
			: m_NextVertexBufferID(0)
		{}

		~VertexBufferManager()
		{}

		VertexBufferID Create()
		{
			VertexBufferID vertexBufferID = m_NextVertexBufferID++;
			m_VertexBuffers.Set(vertexBufferID, std::make_unique<VertexBufferImplementationType>());
			return vertexBufferID;
		}

		VertexBufferImplementationType &Get(const VertexBufferID vertexBufferID)
		{
			GM_ASSERT(vertexBufferID < m_NextVertexBufferID, "Failed to get vertex buffer: invalid ID");
			return m_VertexBuffers.Get(vertexBufferID);
		}
	private:
		VertexBufferID                                                m_NextVertexBufferID;
		ResourceCache<VertexBufferImplementationType, VertexBufferID> m_VertexBuffers;
	};
}