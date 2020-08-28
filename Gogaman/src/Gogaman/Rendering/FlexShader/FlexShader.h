#pragma once

#include "Gogaman/RenderHardwareInterface/DescriptorGroupLayout.h"

namespace Gogaman
{
	namespace FlexShader
	{
		namespace AST { namespace Node { struct Abstract; } }

		//Functions as an intermediate representation. File is loaded, compiled to AST and IR is generated from the AST
		//No concept of entry point. Entry point is just a function name specified at specialization time
		class Module
		{
		public:
			Module(const std::string &source);
			~Module() = default;

			const std::pair<uint32_t, const uint8_t *> CompileKernel(const std::string &entryPointName, const std::vector<std::string> &componentNames = {});
		private:
			std::unordered_map<std::string, RHI::DescriptorGroupLayout> m_ComponentLayouts;

			AST::Node::Abstract *m_RootNode;
		};
	}
}

//Platform-specific. Eg. SPIR-V
//uint8_t *CompileKernel(const Module &module, const std::string &entryPointNames, const std::vector<ComponentTypeID> &types);