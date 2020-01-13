#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractCommandHeap.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		class CommandBuffer;

		class CommandHeap : public AbstractCommandHeap<CommandHeap>
		{
		private:
			struct NativeData
			{
				VkCommandPool vulkanCommandPool;
			};
		public:
			CommandHeap(const Type type);
			CommandHeap(const CommandHeap &) = delete;
			CommandHeap(CommandHeap &&)      = delete;

			~CommandHeap();

			CommandHeap &operator=(const CommandHeap &) = delete;
			CommandHeap &operator=(CommandHeap &&)      = delete;

			std::unique_ptr<CommandBuffer> CreateCommandBuffer()         const;
			std::unique_ptr<CommandBuffer> CreateReusableCommandBuffer() const;

			inline constexpr const NativeData &GetNativeData() const { return m_NativeData; }
			inline constexpr       NativeData &GetNativeData()       { return m_NativeData; }
		private:
			NativeData m_NativeData;
		private:
			//friend AbstractCommandHeap;
		};
	}
}