#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractTexture.h"

#include "Gogaman/RenderHardwareInterface/DeviceMemory.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		class Texture : public AbstractTexture<Texture>
		{
		private:
			struct NativeData
			{
				VkImage                  vulkanImage;
				VkImageView              vulkanImageView;
				DeviceMemory::Allocation vulkanMemory;
			};
		public:
			Texture(const Format format, const uint16_t width = 1, const uint16_t height = 1, const uint16_t depth = 1, const uint8_t levelCount = 1);
			Texture(const Texture &) = delete;
			Texture(Texture &&)      = default;

			~Texture();

			Texture &operator=(const Texture &) = delete;
			Texture &operator=(Texture &&)      = default;

			inline constexpr const NativeData &GetNativeData() const { return m_NativeData; }
			inline constexpr       NativeData &GetNativeData()       { return m_NativeData; }

			static constexpr VkFormat GetNativeFormat(const Format format);
		private:
			NativeData m_NativeData;
		private:
			//friend AbstractTexture;
		};
	}
}