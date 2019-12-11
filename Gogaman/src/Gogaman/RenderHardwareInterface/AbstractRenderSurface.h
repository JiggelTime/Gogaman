#pragma once

#include "Gogaman/Core/CRTP.h"

#include "Gogaman/Core/Base.h"

#include "Identifier.h"

namespace Gogaman
{
	namespace RHI
	{
		template<typename ImplementationType>
		class AbstractRenderSurface : public CRTP<ImplementationType, AbstractRenderSurface>
		{
		public:
			struct Attachment
			{
				TextureID textureID = {};
				uint8_t   mipLevel  = 1;
			};
		public:
			AbstractRenderSurface(const AbstractRenderSurface &) = delete;
			AbstractRenderSurface(AbstractRenderSurface &&)      = default;

			AbstractRenderSurface &operator=(const AbstractRenderSurface &) = delete;
			AbstractRenderSurface &operator=(AbstractRenderSurface &&)      = default;

			inline constexpr const Attachment *GetColorAttachments() const { return m_ColorAttachments; }
			inline constexpr uint8_t GetColorAttachmentCount() const { return m_ColorAttachmentCount; }

			inline constexpr const Attachment &GetDepthStencilAttachment() const { return m_DepthStencilAttachment; }

			inline constexpr uint8_t GetAttachmentCount() const { return GM_IS_INVALID_ID(m_DepthStencilAttachment.textureID) ? m_ColorAttachmentCount : m_ColorAttachmentCount + 1; }

			inline constexpr uint16_t GetWidth()  const { return m_Width;  }
			inline constexpr uint16_t GetHeight() const { return m_Height; }
			inline constexpr uint16_t GetDepth()  const { return m_Depth;  }

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr auto       &GetNativeData()       { return this->GetImplementation().GetNativeData(); }
		private:
			AbstractRenderSurface(const uint8_t colorAttachmentCount, Attachment *colorAttachments, Attachment &&depthStencilAttachment, const uint16_t width = 1, const uint16_t height = 1, const uint16_t depth = 1);

			~AbstractRenderSurface()
			{
				delete[] m_ColorAttachments;
			}
		protected:
			Attachment *m_ColorAttachments;
			Attachment  m_DepthStencilAttachment;
			uint16_t    m_Width, m_Height, m_Depth;
			uint8_t     m_ColorAttachmentCount;
		private:
			friend ImplementationType;
		};
	}
}