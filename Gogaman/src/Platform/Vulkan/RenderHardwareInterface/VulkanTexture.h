#include "Gogaman/RenderHardwareInterface/AbstractTexture.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		GM_STATIC_CLASS_IMPLEMENTATION(Texture, AbstractTexture)
		{
		private:
			struct NativeData
			{
				VkImage        vulkanImage;
				VkDeviceMemory vulkanDeviceMemory;
			};
		public:
			Texture(const Device &device, const uint16_t width = 0, const uint16_t height = 0, const uint16_t depth = 0, const uint8_t levelCount = 1);
			Texture(const Texture &) = delete;
			Texture(Texture &&other) = default;

			~Texture();

			Texture &operator=(const Texture &) = delete;
			Texture &operator=(Texture &&other) = default;

			inline constexpr const NativeData &GetNativeData() const { return m_NativeData; }
			inline constexpr       NativeData &GetNativeData()       { return m_NativeData; }

			static constexpr VkFormat GetNativeFormat(const Format format);
		private:
			NativeData m_NativeData;
		private:
			friend AbstractTexture;
		};
	}
}