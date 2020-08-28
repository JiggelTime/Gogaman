#pragma once

#include "Gogaman/Core/CRTP.h"

namespace Gogaman
{
	namespace FlexShader
	{
		namespace IR { struct IR; }

		template<typename ImplementationType>
		class AbstractCodeGenerator : public CRTP<ImplementationType, AbstractCodeGenerator>
		{
		public:
			inline constexpr const std::pair<uint32_t, const uint8_t *> GetData() const { return this->GetImplementation().GetData(); }
		private:
			AbstractCodeGenerator(const IR::IR &intermediateRepresentation, const std::string &entryPointName)
			{}

			~AbstractCodeGenerator() = default;
		private:
			friend ImplementationType;
		};
	}
}