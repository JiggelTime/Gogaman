#pragma once

#include "Gogaman/Core/CRTP.h"

#include "Gogaman/Core/Base.h"

#include "Device.h"
#include "TextureResource.h"

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
				TextureID textureID = 0;
				uint8_t   mipLevel  = 1;
			};

			struct Attachments
			{
				std::vector<Attachment> colorAttachments;
				Attachment              depthStencilAttachment;
			};
		public:
			AbstractRenderSurface(const AbstractRenderSurface &) = delete;
			AbstractRenderSurface(AbstractRenderSurface &&other) = default;

			AbstractRenderSurface &operator=(const AbstractRenderSurface &) = delete;
			AbstractRenderSurface &operator=(AbstractRenderSurface &&other) = default;

			inline constexpr const Attachments &GetAttachments() const { return m_Attachments; }

			inline constexpr uint16_t GetWidth()  const { return m_Width;  }
			inline constexpr uint16_t GetHeight() const { return m_Height; }
			inline constexpr uint16_t GetDepth()  const { return m_Depth;  }
		protected:
			inline void SetColorAttachment(const uint8_t attachmentIndex, const Attachment &attachment) { this->GetImplementation().SetColorAttachment(attachmentIndex, attachment); }

			inline void SetDepthStencilAttachment(const Attachment &attachment) { this->GetImplementation().SetDepthStencilAttachment(attachment); }
		private:
			AbstractRenderSurface(const Device &device, Attachments &&attachments, const uint16_t width = 0, const uint16_t height = 0, const uint16_t depth = 0)
				: m_Attachments(std::move(attachments)), m_Width(width), m_Height(height), m_Depth(depth)
			{
				#if GM_RHI_DEBUGGING_ENABLED
					GM_ASSERT(m_Attachments.colorAttachments.size() > 0,                                                  "Failed to construct render surface | Invalid attachments | Color attachment count is 0");
					GM_ASSERT(m_Attachments.colorAttachments.size() < device.GetRenderSurfaceColorAttachmentCountLimit(), "Failed to construct render surface | Invalid attachments | Color attachment count exceeds device limit");
					for(const Attachment &i : m_Attachments.colorAttachments)
					{
						GM_ASSERT(i.textureID.index != 0, "Failed to construct render surface | Invalid attachments | Invalid color attachment texture ID");
					}

					GM_ASSERT(m_Width  < device.GetRenderSurfaceWidthLimit(),  "Failed to construct render surface | Width exceeds device limit");
					GM_ASSERT(m_Height < device.GetRenderSurfaceHeightLimit(), "Failed to construct render surface | Height exceeds device limit");
					GM_ASSERT(m_Depth  < device.GetRenderSurfaceDepthLimit(),  "Failed to construct render surface | Depth exceeds device limit");
				#endif
			}

			~AbstractRenderSurface() = default;
		protected:
			Attachments m_Attachments;
			uint16_t    m_Width, m_Height, m_Depth;
		private:
			friend ImplementationType;
		};
	}
}