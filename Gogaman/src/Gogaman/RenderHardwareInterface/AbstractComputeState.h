#pragma once

#include "Gogaman/Core/CRTP.h"

#include "Identifier.h"

namespace Gogaman
{
	namespace RHI
	{
		class DescriptorGroupLayout;

		template<typename ImplementationType>
		class AbstractComputeState : public CRTP<ImplementationType, AbstractComputeState>
		{
		public:
			AbstractComputeState(const AbstractComputeState &) = delete;
			AbstractComputeState(AbstractComputeState &&)      = delete;

			AbstractComputeState &operator=(const AbstractComputeState &) = delete;
			AbstractComputeState &operator=(AbstractComputeState &&)      = delete;

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr       auto &GetNativeData()       { return this->GetImplementation().GetNativeData(); }
		private:
			AbstractComputeState(const std::vector<DescriptorGroupLayout> &descriptorGroupLayouts, const ShaderProgramID shaderProgramID)
				: m_ShaderProgramID(shaderProgramID)
			{}

			~AbstractComputeState() = default;
		protected:
			ShaderProgramID m_ShaderProgramID;
		private:
			friend ImplementationType;
		};
	}
}