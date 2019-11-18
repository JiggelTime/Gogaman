#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractRenderSurface.h"

#include "Gogaman/Core/Base.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		GM_STATIC_CLASS_IMPLEMENTATION(RenderSurface, AbstractRenderSurface)
		{
		private:
			struct NativeData
			{
				VkRenderPass  vulkanRenderPass;
				VkFramebuffer vulkanFramebuffer;
			};
		public:
			RenderSurface(Attachments &&attachments, const uint16_t width = 0, const uint16_t height = 0, const uint16_t depth = 0);
			RenderSurface(const RenderSurface &) = delete;
			RenderSurface(RenderSurface &&other) = default;

			~RenderSurface();

			RenderSurface &operator=(const RenderSurface &) = delete;
			RenderSurface &operator=(RenderSurface &&other) = default;

			inline constexpr const NativeData &GetNativeData() const { return m_NativeData; }
			inline constexpr       NativeData &GetNativeData()       { return m_NativeData; }
		private:
			NativeData m_NativeData;
		private:
			friend AbstractRenderSurface;
		};
	}
}