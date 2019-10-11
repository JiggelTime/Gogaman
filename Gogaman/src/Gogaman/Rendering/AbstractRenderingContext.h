#pragma once

#include "Gogaman/Core/CRTP.h"

#include "RenderSurface.h"
//#include "Shader/Shader.h"
#include "Texture/Texture1D.h"
#include "Texture/Texture2D.h"
#include "Texture/Texture3D.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArrayBuffer.h"

#include "Gogaman/Container.h"

#include "RenderState.h"

namespace Gogaman
{
	using RenderSurfaceID            = ContainerID<uint8_t>;
	using RenderSurfaceContainer     = StaticContainer<255,   RenderSurface,     uint8_t>;
	
	//using ShaderID                   = ContainerID<uint32_t, 24, 8>;
	//using ShaderContainer            = Container<256, Shader, uint32_t, 24, 8>;

	using Texture1D_ID               = ContainerID<uint16_t>;
	using Texture1D_Container        = DynamicContainer<8192, Texture1D,         uint16_t>;

	using Texture2D_ID               = ContainerID<uint16_t>;
	using Texture2D_Container        = DynamicContainer<8192, Texture2D,         uint16_t>;

	using Texture3D_ID               = ContainerID<uint16_t>;
	using Texture3D_Container        = DynamicContainer<8192, Texture3D,         uint16_t>;

	using VertexBufferID             = ContainerID<uint16_t>;
	using VertexBufferContainer      = DynamicContainer<4096, VertexBuffer,      uint16_t>;

	using IndexBufferID              = ContainerID<uint16_t>;
	using IndexBufferContainer       = DynamicContainer<4096, IndexBuffer,       uint16_t>;

	using VertexArrayBufferID        = ContainerID<uint16_t>;
	using VertexArrayBufferContainer = DynamicContainer<4096, VertexArrayBuffer, uint16_t>;

	template<typename ImplementationType>
	class AbstractRenderingContext : public CRTP<ImplementationType, AbstractRenderingContext>
	{
	public:
		inline void Initialize() { this->GetImplementation().Initialize(); }

		inline void SetViewport(const uint16_t width, const uint16_t height) const { this->GetImplementation().SetViewport(width, height); }

		inline void SwapBuffers() { this->GetImplementation().SwapBuffers(); }

		inline void RenderIndexed(const uint32_t indexCount) const { this->GetImplementation().RenderIndexed(indexCount); }
		
		inline void DispatchComputeShader(const uint16_t threadGroupCount) const { DispatchComputeShader(threadGroupCount, threadGroupCount, threadGroupCount); }
		inline void DispatchComputeShader(const uint16_t threadGroupCountX, const uint16_t threadGroupCountY, const uint16_t threadGroupCountZ) const { this->GetImplementation().DispatchComputeShader(threadGroupCountX, threadGroupCountY, threadGroupCountZ); }
		
		inline void EnableDepthTesting()  const { this->GetImplementation().EnableDepthTesting();  }
		inline void DisableDepthTesting() const { this->GetImplementation().DisableDepthTesting(); }

		inline void SetDepthComparisonMode(const RenderState::DepthComparisonMode comparisonMode) const { this->GetImplementation().SetDepthComparisonMode(comparisonMode); }

		inline void SetBlendState(const RenderState::BlendFactor sourceBlendFactor, const RenderState::BlendFactor destinationBlendFactor) const { this->GetImplementation().SetBlendState(sourceBlendFactor, destinationBlendFactor); }
	
		inline void SetFaceWindingOrder(const RenderState::FaceWindingOrder windingOrder) const { this->GetImplementation().SetFaceWindingOrder(windingOrder); }

		inline constexpr RenderSurfaceContainer     &GetRenderSurfaces()     { return m_RenderSurfaces;     }
		//inline constexpr ShaderContainer            &GetShaders()            { return m_Shaders;            }
		inline constexpr Texture1D_Container        &GetTexture1Ds()         { return m_Texture1Ds;         }
		inline constexpr Texture2D_Container        &GetTexture2Ds()         { return m_Texture2Ds;         }
		inline constexpr Texture3D_Container        &GetTexture3Ds()         { return m_Texture3Ds;         }
		inline constexpr VertexBufferContainer      &GetVertexBuffers()      { return m_VertexBuffers;      }
		inline constexpr IndexBufferContainer       &GetIndexBuffers()       { return m_IndexBuffers;       }
		inline constexpr VertexArrayBufferContainer &GetVertexArrayBuffers() { return m_VertexArrayBuffers; }
	protected:
		AbstractRenderingContext()  = default;
		~AbstractRenderingContext() = default;
	private:
		RenderSurfaceContainer     m_RenderSurfaces;
		//ShaderContainer            m_Shaders;
		Texture1D_Container        m_Texture1Ds;
		Texture2D_Container        m_Texture2Ds;
		Texture3D_Container        m_Texture3Ds;
		VertexBufferContainer      m_VertexBuffers;
		IndexBufferContainer       m_IndexBuffers;
		VertexArrayBufferContainer m_VertexArrayBuffers;
	};
}