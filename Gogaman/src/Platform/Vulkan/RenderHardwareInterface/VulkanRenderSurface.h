#include "Gogaman/RenderHardwareInterface/AbstractRenderSurface.h"

#include "Gogaman/Core/Base.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		GM_STATIC_CLASS_IMPLEMENTATION(RenderSurface, AbstractRenderSurface)
		{
		public:
			RenderSurface(const Device &device, Attachments &&attachments, const uint16_t width = 0, const uint16_t height = 0, const uint16_t depth = 0);
			RenderSurface(const RenderSurface &) = delete;
			RenderSurface(RenderSurface &&other) = default;

			~RenderSurface();

			RenderSurface &operator=(const RenderSurface &) = delete;
			RenderSurface &operator=(RenderSurface &&other) = default;

			void SetColorAttachment(const uint8_t attachmentIndex, const Attachment &attachment);

			void SetDepthStencilAttachment(const Attachment &attachment);
		private:
			VkRenderPass  m_VulkanRenderPass;
			VkFramebuffer m_VulkanFramebuffer;
		private:
			friend AbstractRenderSurface;
		};
	}
}