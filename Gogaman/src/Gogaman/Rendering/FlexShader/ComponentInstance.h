#pragma once

#include "Gogaman/RenderHardwareInterface/DescriptorGroupLayout.h"
#include "Gogaman/RenderHardwareInterface/DescriptorGroup.h"

namespace Gogaman
{
	namespace FlexShader
	{
		using ComponentTypeID = uint8_t;

		//Hashmap: key = typeName, value = typeID
		//ModuleVisitor will populate this using ComponentDeclarations
		//Will use to lookup layout when creating a component instance. Possibly for patching function calls during specialization too?

		struct Component
		{

			std::unique_ptr<RHI::DescriptorGroupLayout> descriptorGroupLayout;
		};

		class ComponentInstance
		{
		public:
			ComponentInstance(const std::string &name)
			{

			}

			std::unique_ptr<RHI::DescriptorGroup> descriptorGroup;
			//String?
			std::string type;
		};
	}
}