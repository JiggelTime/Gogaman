#include "Gogaman/RenderHardwareInterface/AbstractRenderContext.h"

#include "Gogaman/Core/Base.h"

namespace Gogaman
{
	namespace RHI
	{
		GM_CLASS_IMPLEMENTATION(VulkanRenderContext, AbstractRenderContext)
		{
		public:
			VulkanRenderContext();
			VulkanRenderContext(const VulkanRenderContext &) = delete;
			VulkanRenderContext(VulkanRenderContext &&other) = default;

			~VulkanRenderContext();

			VulkanRenderContext &operator=(const VulkanRenderContext &) = delete;
			VulkanRenderContext &operator=(VulkanRenderContext &&other) = default;
		private:
			friend AbstractRenderContext;
		};
	}
}