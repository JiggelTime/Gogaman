#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractCommandBuffer.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		class CommandBuffer : public AbstractCommandBuffer<CommandBuffer>
		{
		private:
			struct NativeData
			{
				VkCommandBuffer vulkanCommandBuffer;
			};
		public:
			CommandBuffer()                      = default;
			CommandBuffer(const CommandBuffer &) = delete;
			CommandBuffer(CommandBuffer &&)      = delete;

			~CommandBuffer() = default;

			CommandBuffer &operator=(const CommandBuffer &) = delete;
			CommandBuffer &operator=(CommandBuffer &&)      = delete;

			void StartRecording();
			void StopRecording();

			inline constexpr const NativeData &GetNativeData() const { return m_NativeData; }
			inline constexpr       NativeData &GetNativeData()       { return m_NativeData; }
		private:
			NativeData m_NativeData;
		private:
			//friend AbstractCommandBuffer;
		};
	}
}