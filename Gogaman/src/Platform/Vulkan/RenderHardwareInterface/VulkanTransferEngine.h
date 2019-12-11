#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractTransferEngine.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		class TransferEngine : public AbstractTransferEngine<TransferEngine>
		{
		private:
			struct NativeData
			{
			};
		public:
			TransferEngine();
			TransferEngine(const TransferEngine &) = delete;
			TransferEngine(TransferEngine &&)      = delete;

			~TransferEngine();

			TransferEngine &operator=(const TransferEngine &) = delete;
			TransferEngine &operator=(TransferEngine &&)      = delete;

			inline constexpr const NativeData &GetNativeData() const { return m_NativeData; }
			inline constexpr       NativeData &GetNativeData()       { return m_NativeData; }
		private:
			NativeData m_NativeData;
		private:
			//friend AbstractTransferEngine;
		};
	}
}