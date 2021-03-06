#pragma once

#include "Gogaman/Core/CRTP.h"

#include "DescriptorHeap.h"

#include "Shader.h"

namespace Gogaman
{
	namespace RHI
	{
		template<typename ImplementationType>
		class AbstractDescriptorGroupLayout : public CRTP<ImplementationType, AbstractDescriptorGroupLayout>
		{
		public:
			struct Binding
			{
				uint32_t             descriptorCount = 1;
				DescriptorHeap::Type type;
			};
		public:
			AbstractDescriptorGroupLayout(const AbstractDescriptorGroupLayout &) = delete;
			AbstractDescriptorGroupLayout(AbstractDescriptorGroupLayout &&)      = default;

			AbstractDescriptorGroupLayout &operator=(const AbstractDescriptorGroupLayout &) = delete;
			AbstractDescriptorGroupLayout &operator=(AbstractDescriptorGroupLayout &&)      = default;

			inline constexpr const Binding &GetBinding(const uint32_t index) const { return m_Bindings[index]; }

			inline constexpr uint32_t GetBindingCount() const { return m_BindingCount; }

			inline constexpr Shader::StageFlag GetShaderVisibilityFlags() const { return m_ShaderVisibilityFlags; }

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr auto       &GetNativeData()       { return this->GetImplementation().GetNativeData(); }
		private:
			AbstractDescriptorGroupLayout(const uint32_t bindingCount, Binding *bindings, const Shader::StageFlag shaderVisibilityFlags)
				: m_BindingCount(bindingCount), m_Bindings(bindings), m_ShaderVisibilityFlags(shaderVisibilityFlags)
			{}

			~AbstractDescriptorGroupLayout()
			{
				delete[] m_Bindings;
			}
		protected:
			Binding           *m_Bindings;
			uint32_t           m_BindingCount;
			Shader::StageFlag  m_ShaderVisibilityFlags;
		private:
			friend ImplementationType;
		};
	}
}