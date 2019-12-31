#pragma once

#include "Gogaman/Core/CRTP.h"

#include "Identifier.h"

#include "DescriptorHeap.h"
#include "DescriptorGroupLayout.h"

namespace Gogaman
{
	namespace RHI
	{
		class Sampler;

		class Texture;

		template<typename ImplementationType>
		class AbstractDescriptorGroup : public CRTP<ImplementationType, AbstractDescriptorGroup>
		{
		public:
			AbstractDescriptorGroup(const AbstractDescriptorGroup &) = delete;
			AbstractDescriptorGroup(AbstractDescriptorGroup &&)      = default;

			AbstractDescriptorGroup &operator=(const AbstractDescriptorGroup &) = delete;
			AbstractDescriptorGroup &operator=(AbstractDescriptorGroup &&)      = default;

			inline void SetShaderTexture(const uint32_t bindingIndex, const Texture &texture) { this->GetImplementation().SetShaderTexture(bindingIndex, texture); }
			inline void SetShaderConstantBuffer(const uint32_t bindingIndex, const BufferID bufferID) { this->GetImplementation().SetShaderResource(bindingIndex, bufferID); }
			inline void SetStorageResource(const uint32_t bindingIndex) { this->GetImplementation().SetStorageResource(bindingIndex); }
			inline void SetSampler(const uint32_t bindingIndex, const Sampler &sampler) { this->GetImplementation().SetSampler(bindingIndex, sampler); }

			inline constexpr const DescriptorGroupLayout *GetLayout() const { return m_Layout; }

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr auto       &GetNativeData()       { return this->GetImplementation().GetNativeData(); }
		private:
			AbstractDescriptorGroup(DescriptorHeap *heap, DescriptorGroupLayout *layout)
				: m_Heap(heap), m_Layout(layout)
			{}

			~AbstractDescriptorGroup() = default;
		protected:
			DescriptorHeap        *m_Heap;
			DescriptorGroupLayout *m_Layout;
		private:
			friend ImplementationType;
		};
	}
}