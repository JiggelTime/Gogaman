#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractRenderSurface.h"

#include "Gogaman/Core/Base.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		class RenderSurface : public AbstractRenderSurface<RenderSurface>
		{
		private:
			struct NativeData
			{
				VkRenderPass  vulkanRenderPass;
				VkFramebuffer vulkanFramebuffer;
			};
		public:
			RenderSurface(const uint8_t colorAttachmentCount, Attachment *colorAttachments, Attachment &&depthStencilAttachment, const uint16_t width = 1, const uint16_t height = 1, const uint16_t depth = 1);
			RenderSurface(const RenderSurface &) = delete;
			RenderSurface(RenderSurface &&)      = default;

			~RenderSurface();

			RenderSurface &operator=(const RenderSurface &) = delete;
			RenderSurface &operator=(RenderSurface &&)      = default;

			inline constexpr const NativeData &GetNativeData() const { return m_NativeData; }
			inline constexpr       NativeData &GetNativeData()       { return m_NativeData; }
		private:
			NativeData m_NativeData;
		private:
			//friend AbstractRenderSurface;
		};
	}
}