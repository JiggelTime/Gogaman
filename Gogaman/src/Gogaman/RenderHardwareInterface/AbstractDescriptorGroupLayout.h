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
				DescriptorHeap::Type type;
				uint32_t             index;
				uint32_t             descriptorCount;
			};
		public:
			AbstractDescriptorGroupLayout(const AbstractDescriptorGroupLayout &) = delete;
			AbstractDescriptorGroupLayout(AbstractDescriptorGroupLayout &&)      = default;

			AbstractDescriptorGroupLayout &operator=(const AbstractDescriptorGroupLayout &) = delete;
			AbstractDescriptorGroupLayout &operator=(AbstractDescriptorGroupLayout &&)      = default;

			inline constexpr const std::vector<Binding> &GetBindings() const { return m_Bindings; }

			inline constexpr Shader::StageFlags GetShaderVisibilityFlags() const { return m_ShaderVisibilityFlags; }

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr auto       &GetNativeData()       { return this->GetImplementation().GetNativeData(); }
		private:
			AbstractDescriptorGroupLayout(std::vector<Binding> &&bindings, const Shader::StageFlags shaderVisibilityFlags)
				: m_Bindings(std::move(bindings)), m_ShaderVisibilityFlags(shaderVisibilityFlags)
			{}

			~AbstractDescriptorGroupLayout() = default;
		protected:
			std::vector<Binding> m_Bindings;
			Shader::StageFlags   m_ShaderVisibilityFlags;
		private:
			friend ImplementationType;
		};
	}
}