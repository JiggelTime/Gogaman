#pragma once

#include "Gogaman/Core/CRTP.h"

#include "Gogaman/Core/Base.h"

#include "Gogaman/Core/ResourceContainer.h"

namespace Gogaman
{
	namespace RHI
	{
		//using TextureID = ResourceContainer<Texture, uint8_t, 256>::ID;

		template<typename ImplementationType>
		class AbstractRenderSurface : public CRTP<ImplementationType, AbstractRenderSurface>
		{
		public:
			struct Attachment
			{
				ResourceID<uint8_t> textureID;
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

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr auto       &GetNativeData()       { return this->GetImplementation().GetNativeData(); }
		private:
			AbstractRenderSurface(Attachments &&attachments, const uint16_t width = 0, const uint16_t height = 0, const uint16_t depth = 0);

			~AbstractRenderSurface() = default;
		protected:
			Attachments m_Attachments;
			uint16_t    m_Width, m_Height, m_Depth;
		private:
			friend ImplementationType;
		};
	}
}