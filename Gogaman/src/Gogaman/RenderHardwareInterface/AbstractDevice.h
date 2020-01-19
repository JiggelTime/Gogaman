#pragma once

#include "Gogaman/Core/CRTP.h"

#include "Gogaman/Utilities/Container/ResourceContainer.h"

#include "Identifier.h"

#include "Sampler.h"
#include "Texture.h"
#include "Buffer.h"
#include "RenderSurface.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "CommandBuffer.h"

#include "CommandHeap.h"

//On Android this should be 3 because of heavier pipelining
#define GM_RHI_CONCURRENT_FRAME_COUNT 2u

namespace Gogaman
{
	enum class VerticalSynchronization : uint8_t
	{
		Disabled,
		DoubleBuffered
	};

	namespace RHI
	{
		class Fence;

		using SamplerContainer       = ResourceContainer<Sampler,       SamplerID,       128>;
		using TextureContainer       = ResourceContainer<Texture,       TextureID,       1024>;
		using BufferContainer        = ResourceContainer<Buffer,        BufferID,        1024>;
		using RenderSurfaceContainer = ResourceContainer<RenderSurface, RenderSurfaceID, 32>;
		using ShaderContainer        = ResourceContainer<Shader,        ShaderID,        128>;
		using ShaderProgramContainer = ResourceContainer<ShaderProgram, ShaderProgramID, 32>;
		using CommandBufferContainer = ResourceContainer<CommandBuffer, CommandBufferID, 256>;

		template<typename ImplementationType>
		class AbstractDevice : public CRTP<ImplementationType, AbstractDevice>
		{
		public:
			struct Resources
			{
				SamplerContainer       samplers;
				TextureContainer       textures;
				BufferContainer        buffers;
				RenderSurfaceContainer renderSurfaces;
				ShaderContainer        shaders;
				ShaderProgramContainer shaderPrograms;
				CommandBufferContainer commandBuffers;
			};
		public:
			AbstractDevice(const AbstractDevice &) = delete;
			AbstractDevice(AbstractDevice &&)      = default;

			AbstractDevice &operator=(const AbstractDevice &) = delete;
			AbstractDevice &operator=(AbstractDevice &&)      = default;

			inline void CreateSwapChain(const uint16_t width, const uint16_t height, const VerticalSynchronization verticalSynchronization) { this->GetImplementation().CreateSwapChain(width, height, verticalSynchronization); }

			inline void RecreateSwapChain(const uint16_t width, const uint16_t height, const VerticalSynchronization verticalSynchronization) { this->GetImplementation().RecreateSwapChain(width, height, verticalSynchronization); }

			inline void SubmitCommands(const CommandHeap::Type type, const uint8_t commandBufferCount, CommandBuffer *commandBuffers, Fence *fence = nullptr) { this->GetImplementation().SubmitCommands(type, commandBufferCount, commandBuffers, fence); }
			inline void SubmitCommands(const CommandHeap::Type type, const uint8_t commandBufferCount, CommandBufferID *commandBufferIDs, Fence *fence = nullptr) { this->GetImplementation().SubmitCommands(type, commandBufferCount, commandBufferIDs, fence); }

			inline void Present() { this->GetImplementation().Present(); }

			inline constexpr const Resources &GetResources() const { return m_Resources; }
		    inline constexpr       Resources &GetResources()       { return m_Resources; }

		    inline constexpr const RenderSurfaceID *GetSwapChainRenderSurfaceIDs() const { return m_SwapChainRenderSurfaceIDs; }

			inline constexpr uint32_t GetTextureWidthLimit()  const { return this->GetImplementation().GetTextureWidthLimit();  }
			inline constexpr uint32_t GetTextureHeightLimit() const { return this->GetImplementation().GetTextureHeightLimit(); }
			inline constexpr uint32_t GetTextureDepthLimit()  const { return this->GetImplementation().GetTextureDepthLimit();  }

			inline constexpr uint32_t GetVertexShaderInputAttributeCountLimit() const { return this->GetImplementation().GetVertexShaderInputAttributeCountLimit(); }

			inline constexpr uint32_t GetPixelShaderOutputAttachmentCountLimit() const { return this->GetImplementation().GetPixelShaderOutputAttachmentCountLimit(); }

			inline constexpr const uint32_t *GetComputeShaderWorkGroupCountLimit()      const { return this->GetImplementation().GetComputeShaderWorkGroupCountLimit();      }
			inline constexpr uint32_t        GetComputeShaderWorkGroupInvocationLimit() const { return this->GetImplementation().GetComputeShaderWorkGroupInvocationLimit(); }
			inline constexpr const uint32_t *GetComputeShaderWorkGroupSizeLimit()       const { return this->GetImplementation().GetComputeShaderWorkGroupSizeLimit();       }

			inline constexpr uint32_t GetRenderSurfaceWidthLimit()                const { return this->GetImplementation().GetRenderSurfaceWidthLimit();                }
			inline constexpr uint32_t GetRenderSurfaceHeightLimit()               const { return this->GetImplementation().GetRenderSurfaceHeightLimit();               }
			inline constexpr uint32_t GetRenderSurfaceDepthLimit()                const { return this->GetImplementation().GetRenderSurfaceDepthLimit();                }
			inline constexpr uint32_t GetRenderSurfaceColorAttachmentCountLimit() const { return this->GetImplementation().GetRenderSurfaceColorAttachmentCountLimit(); }

			inline constexpr uint32_t GetViewportCountLimit() const { return this->GetImplementation().GetViewportCountLimit(); }

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr auto       &GetNativeData()       { return this->GetImplementation().GetNativeData(); }

			inline constexpr auto GetNativeCommandHeapType(const CommandHeap::Type type) const { return this->GetImplementation().GetNativeCommandHeapType(type); }
		private:
			AbstractDevice(void *nativeWindow)
			{}

			~AbstractDevice() = default;
		protected:
			Resources       m_Resources;
			RenderSurfaceID m_SwapChainRenderSurfaceIDs[GM_RHI_CONCURRENT_FRAME_COUNT];
		private:
			friend ImplementationType;
		};
	}
}