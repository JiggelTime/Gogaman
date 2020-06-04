#pragma once

namespace Gogaman
{
	namespace FlexShader
	{
		#if 0		
		//Functions as an intermediate representation. File is loaded, compiled to AST and IR is generated from the AST
		//No concept of entry point. Entry point is just a function name. It can be specified at specialization time
		class Module
		{
		public:
			Module(const std::string &source);
			~Module() = default;

			ComponentTypeID GetComponentTypeID(const std::string &name);
			DescriptorSetLayout GetLayout(const ComponentTypeID componentTypeID);
		private:
			std::vector<Component> components;
		};
		#endif
	}
}

//Platform-specific. Eg. SPIR-V
//uint8_t *CompileKernel(const Module &module, const std::string &entryPointNames, const std::vector<ComponentTypeID> &types);