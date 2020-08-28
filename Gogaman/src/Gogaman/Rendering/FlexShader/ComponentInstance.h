#pragma once

#include "Gogaman/RenderHardwareInterface/DescriptorGroupLayout.h"
#include "Gogaman/RenderHardwareInterface/DescriptorGroup.h"

namespace Gogaman
{
	namespace FlexShader
	{
		using ComponentTypeID = uint8_t;

		//Hashmap: key = typeName, value = typeID or Type
		//ModuleVisitor will populate this using Components
		//Will use to lookup layout when creating a component instance. Possibly for patching function calls during specialization too?
		#if 0
		class Component
		{
		public:
			Component(std::unique_ptr<RHI::DescriptorGroupLayout> &&layout)
				: m_Layout(std::move(layout))
			{}
		private:
			std::unique_ptr<RHI::DescriptorGroupLayout> m_Layout;
			//Shader::StageFlag visibility;
		};
		#endif

		/*
		class ComponentInstance
		{
		public:
			ComponentInstance()  = default;
			~ComponentInstance() = default;
		private:
			std::unique_ptr<RHI::DescriptorGroup> m_DescriptorGroup;
			//String?
			std::string m_Type;
		};*/
		#if 0
		struct ComponentInstance
		{
			RHI::DescriptorGroup descriptorGroup;
			std::string          type;
		};
		
		ComponentInstance CreateBasicMaterialInstance(const RHI::Texture &texture)
		{
			ComponentInstance instance{ RHI::DescriptorGroup(descriptorHeap, module.GetComponentLayout("BasicMaterial")), "BasicMaterial" };

			instance.descriptorGroup.SetShaderTexture(0, texture);

			//Constant buffer...
			//instance.descriptorGroup.SetConstantBuffer()

			return instance;
		}
		#endif
	}
}