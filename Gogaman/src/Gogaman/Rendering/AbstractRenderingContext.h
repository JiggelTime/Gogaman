#pragma once

#include "Gogaman/Core/CRTP.h"

#include "RenderSurface.h"
//#include "Shader/Shader.h"
#include "Texture/Texture.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArrayBuffer.h"

#include "Gogaman/Resource.h"

#include "RenderState.h"

namespace Gogaman
{
	using RenderSurfaceID            = Resource::ID<uint32_t, 24, 8>;
	using RenderSurfaceContainer     = Resource::Container<256, IndexBuffer, uint32_t, 24, 8>;
	
	//using ShaderID                   = Resource::ID<uint32_t, 24, 8>;
	//using ShaderContainer            = Resource::Container<256, IndexBuffer, uint32_t, 24, 8>;

	using TextureID                  = Resource::ID<uint32_t, 19, 13>;
	using TextureContainer           = Resource::Container<8192, IndexBuffer, uint32_t, 19, 13>;

	using VertexBufferID             = Resource::ID<uint32_t, 20, 12>;
	using VertexBufferContainer      = Resource::Container<4096, IndexBuffer, uint32_t, 20, 12>;

	using IndexBufferID              = Resource::ID<uint32_t, 20, 12>;
	using IndexBufferContainer       = Resource::Container<4096, IndexBuffer, uint32_t, 20, 12>;

	using VertexArrayBufferID        = Resource::ID<uint32_t, 20, 12>;
	using VertexArrayBufferContainer = Resource::Container<4096, VertexArrayBuffer, uint32_t, 20, 12>;

	template<typename ImplementationType>
	class AbstractRenderingContext : public CRTP<ImplementationType, AbstractRenderingContext>
	{
	public:
		inline void Initialize() { this->GetImplementation().Initialize(); }

		inline void SwapBuffers() { this->GetImplementation().SwapBuffers(); }

		inline void RenderIndexed(const uint32_t indexCount) const { this->GetImplementation().RenderIndexed(indexCount); }
		
		inline void DispatchComputeShader(const uint16_t threadGroupCount) const { DispatchComputeShader(threadGroupCount, threadGroupCount, threadGroupCount); }
		inline void DispatchComputeShader(const uint16_t threadGroupCountX, const uint16_t threadGroupCountY, const uint16_t threadGroupCountZ) const { this->GetImplementation().DispatchComputeShader(threadGroupCountX, threadGroupCountY, threadGroupCountZ); }
		
		inline void SetBlendState(const BlendFactor sourceBlendFactor, const BlendFactor destinationBlendFactor) const { this->GetImplementation().SetBlendState(sourceBlendFactor, destinationBlendFactor); }
	
		inline void SetFaceWindingOrder(const FaceWindingOrder windingOrder) const { this->GetImplementation().SetFaceWindingOrder(windingOrder); }

		inline RenderSurfaceID CreateRenderSurface() { return m_RenderSurfaces.Add(RenderSurface); }
		inline constexpr RenderSurface &GetRenderSurface(const RenderSurfaceID renderSurfaceID) { return m_RenderSurfaces.Get(renderSurfaceID); }
		inline void DeleteRenderSurface(const RenderSurfaceID renderSurfaceID) { m_RenderSurfaces.Erase(renderSurfaceID); }
		inline constexpr auto GetRenderSurfaceCount() { return m_RenderSurfaces.GetResourceCount(); }

		//inline ShaderID CreateShader() { return m_Shaders.Add(Shader); }
		//inline constexpr Shader &GetShader(const ShaderID shaderID) { return m_Shaders.Get(shaderID); }
		//inline void DeleteShader(const ShaderID shaderID) { m_Shaders.Erase(shaderID); }
		//inline constexpr auto GetShaderCount() { return m_Shaders.GetResourceCount(); }

		inline TextureID CreateTexture() { return m_Textures.Add(Texture); }
		inline constexpr Texture &GetTexture(const TextureID textureID) { return m_Textures.Get(textureID); }
		inline void DeleteTexture(const TextureID textureID) { m_Textures.Erase(textureID); }
		inline constexpr auto GetTextureCount() { return m_Textures.GetResourceCount(); }

		inline VertexBufferID CreateVertexBuffer() { return m_VertexBuffers.Add(VertexBuffer); }
		inline constexpr VertexBuffer &GetVertexBuffer(const VertexBufferID vertexBufferID) { return m_VertexBuffers.Get(vertexBufferID); }
		inline void DeleteVertexBuffer(const VertexBufferID vertexBufferID) { m_VertexBuffers.Erase(vertexBufferID); }
		inline constexpr auto GetVertexBufferCount() { return m_VertexBuffers.GetResourceCount(); }

		inline IndexBufferID CreateIndexBuffer() { return m_IndexBuffers.Add(IndexBuffer); }
		inline constexpr IndexBuffer &GetIndexBuffer(const IndexBufferID indexBufferID) { return m_IndexBuffers.Get(indexBufferID); }
		inline void DeleteIndexBuffer(const IndexBufferID indexBufferID) { m_IndexBuffers.Erase(indexBufferID); }
		inline constexpr auto GetIndexBufferCount() { return m_IndexBuffers.GetResourceCount(); }

		inline VertexArrayBufferID CreateVertexArrayBuffer() { return m_VertexArrayBuffers.Add(VertexArrayBuffer); }
		inline constexpr VertexArrayBuffer &GetVertexArrayBuffer(const VertexArrayBufferID vertexArrayBufferID) { return m_VertexArrayBuffers.Get(vertexArrayBufferID); }
		inline void DeleteVertexArrayBuffer(const VertexArrayBufferID vertexArrayBufferID) { m_VertexArrayBuffers.Erase(vertexArrayBufferID); }
		inline constexpr auto GetVertexArrayBufferCount() { return m_VertexArrayBuffers.GetResourceCount(); }

	protected:
		AbstractRenderingContext()  = default;
		~AbstractRenderingContext() = default;
	private:
		RenderSurfaceContainer     m_RenderSurfaces;
		//ShaderContainer            m_Shaders;
		TextureContainer           m_Textures;
		VertexBufferContainer      m_VertexBuffers;
		IndexBufferContainer       m_IndexBuffers;
		VertexArrayBufferContainer m_VertexArrayBuffers;
	};
}