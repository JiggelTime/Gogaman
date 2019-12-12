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
				uint32_t             index;
				uint32_t             descriptorCount;
				DescriptorHeap::Type type;
			};
		public:
			AbstractDescriptorGroupLayout(const AbstractDescriptorGroupLayout &) = delete;
			AbstractDescriptorGroupLayout(AbstractDescriptorGroupLayout &&)      = default;

			AbstractDescriptorGroupLayout &operator=(const AbstractDescriptorGroupLayout &) = delete;
			AbstractDescriptorGroupLayout &operator=(AbstractDescriptorGroupLayout &&)      = default;

			inline constexpr const Binding *GetBindings() const { return m_Bindings; }

			inline constexpr uint32_t GetBindingCount() const { return m_BindingCount; }

			inline constexpr Shader::StageFlags GetShaderVisibilityFlags() const { return m_ShaderVisibilityFlags; }

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr auto       &GetNativeData()       { return this->GetImplementation().GetNativeData(); }
		private:
			AbstractDescriptorGroupLayout(const uint32_t bindingCount, Binding *bindings, const Shader::StageFlags shaderVisibilityFlags)
				: m_BindingCount(bindingCount), m_Bindings(bindings), m_ShaderVisibilityFlags(shaderVisibilityFlags)
			{}

			~AbstractDescriptorGroupLayout()
			{
				delete[] m_Bindings;
			}
		protected:
			Binding            *m_Bindings;
			uint32_t            m_BindingCount;
			Shader::StageFlags  m_ShaderVisibilityFlags;
		private:
			friend ImplementationType;
		};
	}
}