#pragma once

#include "Gogaman/Core/CRTP.h"

#include "Gogaman/Core/ResourceContainer.h"

#include "Identifier.h"

#include "Context.h"

#include "Texture.h"
#include "Buffer.h"
#include "RenderSurface.h"
#include "Shader.h"
#include "ShaderProgram.h"

#include "CommandHeap.h"

#define GM_SWAP_CHAIN_BUFFER_COUNT 2u

namespace Gogaman
{
	enum class VerticalSynchronization : uint8_t
	{
		Disabled,
		DoubleBuffered
	};

	namespace RHI
	{
		class CommandBuffer;

		using TextureContainer       = ResourceContainer<Texture,       TextureID,       1024>;
		using BufferContainer        = ResourceContainer<Buffer,        BufferID,        1024>;
		using RenderSurfaceContainer = ResourceContainer<RenderSurface, RenderSurfaceID, 32>;
		using ShaderContainer        = ResourceContainer<Shader,        ShaderID,        128>;
		using ShaderProgramContainer = ResourceContainer<ShaderProgram, ShaderProgramID, 32>;

		template<typename ImplementationType>
		class AbstractDevice : public CRTP<ImplementationType, AbstractDevice>
		{
		public:
			struct Resources
			{
				TextureContainer       textures;
				BufferContainer        buffers;
				RenderSurfaceContainer renderSurfaces;
				ShaderContainer        shaders;
				ShaderProgramContainer shaderPrograms;
			};
		public:
			AbstractDevice(const AbstractDevice &) = delete;
			AbstractDevice(AbstractDevice &&)      = default;

			AbstractDevice &operator=(const AbstractDevice &) = delete;
			AbstractDevice &operator=(AbstractDevice &&)      = default;

			inline void CreateSwapChain(const uint16_t width, const uint16_t height, const VerticalSynchronization verticalSynchronization) { this->GetImplementation().CreateSwapChain(width, height, verticalSynchronization); }

			inline void RecreateSwapChain(const uint16_t width, const uint16_t height, const VerticalSynchronization verticalSynchronization) { this->GetImplementation().RecreateSwapChain(width, height, verticalSynchronization); }

			inline void SubmitTransferCommands(const uint8_t commandBufferCount, CommandBuffer *commandBuffers) { this->GetImplementation().SubmitTransferCommands(commandBufferCount, commandBuffers); }
			inline void SubmitDirectCommands(const uint8_t commandBufferCount, CommandBuffer *commandBuffers) { this->GetImplementation().SubmitDirectCommands(commandBufferCount, commandBuffers); }

			inline void Present() { this->GetImplementation().Present(); }

			inline constexpr const Context &GetContext() const { return m_Context; }
			inline constexpr       Context &GetContext()       { return m_Context; }

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
		private:
			Context         m_Context;
			Resources       m_Resources;
			RenderSurfaceID m_SwapChainRenderSurfaceIDs[GM_SWAP_CHAIN_BUFFER_COUNT];
		private:
			friend ImplementationType;
		};
	}
}