#pragma once

#include "Gogaman/Core/CRTP.h"

#include "Gogaman/Core/ResourceContainer.h"

#include "Context.h"

#include "Texture.h"
#include "RenderSurface.h"

namespace Gogaman
{
	enum class VerticalSynchronization : uint8_t
	{
		Disabled,
		DoubleBuffered
	};

	namespace RHI
	{
		enum class CommandQueueType : uint8_t
		{
			Compute,
			Transfer,
			Direct
		};

		//using TextureID        = TextureContainer::ID;
		using TextureContainer = ResourceContainer<Texture, ResourceID<uint8_t>, 256>;

		using RenderSurfaceContainer = ResourceContainer<RenderSurface, ResourceID<uint8_t>, 32>;
		//using RenderSurfaceID        = RenderSurfaceContainer::ID;

		template<typename ImplementationType>
		class AbstractDevice : public CRTP<ImplementationType, AbstractDevice>
		{
		public:
			struct Resources
			{
				TextureContainer       textures;
				RenderSurfaceContainer renderSurfaces;
			};
		public:
			AbstractDevice(const AbstractDevice &) = delete;
			AbstractDevice(AbstractDevice &&)      = default;

			AbstractDevice &operator=(const AbstractDevice &) = delete;
			AbstractDevice &operator=(AbstractDevice &&)      = default;

			inline void CreateSwapChain(const uint16_t width, const uint16_t height, const VerticalSynchronization verticalSynchronization) { this->GetImplementation().CreateSwapChain(width, height, verticalSynchronization); }

			inline void RecreateSwapChain(const uint16_t width, const uint16_t height, const VerticalSynchronization verticalSynchronization) { this->GetImplementation().RecreateSwapChain(width, height, verticalSynchronization); }

			inline constexpr const Context &GetContext() const { return m_Context; }
			inline constexpr       Context &GetContext()       { return m_Context; }

			inline constexpr const Resources &GetResources() const { return m_Resources; }
		    inline constexpr       Resources &GetResources()       { return m_Resources; }

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr auto       &GetNativeData()       { return this->GetImplementation().GetNativeData(); }

			inline constexpr auto GetNativeCommandQueueType(const CommandQueueType type) const { return this->GetImplementation().GetNativeCommandQueueType(type); }

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
		private:
			AbstractDevice(void *nativeWindow)
			{}

			~AbstractDevice() = default;
		private:
			Context   m_Context;
			Resources m_Resources;
		private:
			friend ImplementationType;
		};
	}
}