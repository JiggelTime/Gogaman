#pragma once

#include "Gogaman/Core/CRTP.h"

#include "DescriptorHeap.h"
#include "DescriptorGroupLayout.h"

#include "Shader.h"

namespace Gogaman
{
	namespace RHI
	{
		template<typename ImplementationType>
		class AbstractDescriptorGroup : public CRTP<ImplementationType, AbstractDescriptorGroup>
		{
		public:
			AbstractDescriptorGroup(const AbstractDescriptorGroup &) = delete;
			AbstractDescriptorGroup(AbstractDescriptorGroup &&)      = default;

			AbstractDescriptorGroup &operator=(const AbstractDescriptorGroup &) = delete;
			AbstractDescriptorGroup &operator=(AbstractDescriptorGroup &&)      = default;

			inline void SetShaderResource(const uint32_t index)  { this->GetImplementation().SetShaderResource(index);  }
			inline void SetStorageResource(const uint32_t index) { this->GetImplementation().SetStorageResource(index); }
			inline void SetSampler(const uint32_t index)         { this->GetImplementation().SetSampler(index);         }

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr auto       &GetNativeData()       { return this->GetImplementation().GetNativeData(); }
		private:
			AbstractDescriptorGroup(DescriptorHeap *heap, const DescriptorGroupLayout &layout)
				: m_Heap(heap)
			{}

			~AbstractDescriptorGroup() = default;
		private:
			DescriptorHeap *m_Heap;
		private:
			friend ImplementationType;
		};
	}
}