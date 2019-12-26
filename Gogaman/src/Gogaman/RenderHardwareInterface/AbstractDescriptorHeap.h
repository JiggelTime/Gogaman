#pragma once

#include "Gogaman/Core/CRTP.h"

#define GM_DESCRIPTOR_HEAP_TYPE_COUNT 9

namespace Gogaman
{
	namespace RHI
	{
		template<typename ImplementationType>
		class AbstractDescriptorHeap : public CRTP<ImplementationType, AbstractDescriptorHeap>
		{
		public:
			enum class Type : uint8_t
			{
				ShaderTexture,
				StorageTexture,

				ShaderTexelBuffer,
				StorageTexelBuffer,
				ShaderConstantBuffer,
				StorageConstantBuffer,
				DepthStencilBuffer,
				RenderTargetBuffer,

				Sampler
			};

			struct DescriptorCounts
			{
				template<Type type>
				inline void SetDescriptorCount(const uint32_t count)
				{
					uint32_t &descriptorCount = descriptorCounts[(uint8_t)type];

					totalDescriptorCount += count - descriptorCount;
					descriptorCount = count;
				}

				template<Type type>
				inline constexpr uint32_t GetDescriptorCount() const { return descriptorCounts[(uint8_t)type]; }

				uint32_t descriptorCounts[GM_DESCRIPTOR_HEAP_TYPE_COUNT] = {};
				uint32_t totalDescriptorCount                            = 0;
			};
		public:
			AbstractDescriptorHeap(const AbstractDescriptorHeap &) = delete;
			AbstractDescriptorHeap(AbstractDescriptorHeap &&)      = delete;

			AbstractDescriptorHeap &operator=(const AbstractDescriptorHeap &) = delete;
			AbstractDescriptorHeap &operator=(AbstractDescriptorHeap &&)      = delete;

			inline constexpr const DescriptorCounts &GetDescriptorCounts() const { return m_DescriptorCounts; }

			static inline constexpr auto GetNativeType(const Type type) { return ImplementationType::GetNativeType(type); }
		private:
			AbstractDescriptorHeap(DescriptorCounts &&descriptorCounts)
				: m_DescriptorCounts(std::move(descriptorCounts))
			{}

			~AbstractDescriptorHeap() = default;
		private:
			DescriptorCounts m_DescriptorCounts;
		private:
			friend ImplementationType;
		};
	}
}