#pragma once

#include "Gogaman/Core/Base.h"

namespace Gogaman
{
	namespace FlexShader
	{
		enum class Type : uint8_t
		{
			Void,
			Boolean,
			Integer,
			Integer2,
			Integer3,
			Integer4,
			Float,
			Float2,
			Float3,
			Float4,
			Float2x2,
			Float3x3,
			Float4x4,
			Sampler,
			Count
		};

		inline Type GetType(const std::string &typeName)
		{
			if(typeName == "void")
				return Type::Void;
			else if(typeName == "bool")
				return Type::Boolean;
			else if(typeName == "int")
				return Type::Integer;
			else if(typeName == "int2")
				return Type::Integer2;
			else if(typeName == "int3")
				return Type::Integer3;
			else if(typeName == "int4")
				return Type::Integer4;
			else if(typeName == "float")
				return Type::Float;
			else if(typeName == "float2")
				return Type::Float2;
			else if(typeName == "float3")
				return Type::Float3;
			else if(typeName == "float4")
				return Type::Float4;
			else if(typeName == "float2x2")
				return Type::Float2x2;
			else if(typeName == "float3x3")
				return Type::Float3x3;
			else if(typeName == "float4x4")
				return Type::Float4x4;
			else if(typeName == "Sampler")
				return Type::Sampler;
		}

		inline bool IsTypeName(const std::string &typeName) { return ((typeName == "void") || (typeName == "bool") || (typeName == "int") || (typeName == "int2") || (typeName == "int3") || (typeName == "int4") || (typeName == "float") || (typeName == "float2") || (typeName == "float3") || (typeName == "float4") || (typeName == "float2x2") || (typeName == "float3x3") || (typeName == "float4x4") || (typeName == "Sampler")); }

		inline std::string GetTypeString(const Type type)
		{
			const std::string names[]
			{
				"void",
				"bool",
				"int",
				"int2",
				"int3",
				"int4",
				"float",
				"float2",
				"float3",
				"float4",
				"float2x2",
				"float3x3",
				"float4x4",
				"Sampler"
			};

			return std::string(names[(uint8_t)type]);
		}

		inline uint8_t GetTypeComponentCount(const Type type)
		{
			switch(type)
			{
			case Type::Void:
				return 0;
			case Type::Boolean:
				return 1;
			case Type::Integer:
				return 1;
			case Type::Integer2:
				return 2;
			case Type::Integer3:
				return 3;
			case Type::Integer4:
				return 4;
			case Type::Float:
				return 1;
			case Type::Float2:
				return 2;
			case Type::Float3:
				return 3;
			case Type::Float4:
				return 4;
			case Type::Float2x2:
				return 2 * 2;
			case Type::Float3x3:
				return 3 * 3;
			case Type::Float4x4:
				return 4 * 4;
			default:
				GM_DEBUG_ASSERT(false, "Failed to get FlexShader type component count | Type \"%s\" is invalid", GetTypeString(type).c_str());
			}
		}

		enum class VariableSpecifier : uint8_t
		{
			DescriptorGroup0,
			DescriptorGroup1,
			DescriptorGroup2,
			DescriptorGroup3,
			DescriptorGroup4,
			DescriptorGroup5,
			DescriptorGroup6,
			DescriptorGroup7,
			DescriptorGroup8,
			DescriptorGroup9,
			DescriptorGroup10,
			DescriptorGroup11,
			DescriptorGroup12,
			DescriptorGroup13,
			DescriptorGroup14,
			DescriptorGroup15,

			None,
			Input,
			Output
		};

		inline VariableSpecifier GetVariableSpecifier(const std::string &specifierName)
		{
			if(specifierName == "input")
				return VariableSpecifier::Input;
			else if(specifierName == "output")
				return VariableSpecifier::Output;
			else
				return VariableSpecifier::None;
		}

		inline std::string GetVariableSpecifierString(const VariableSpecifier specifier)
		{
			const std::string names[]
			{
				"DescriptorGroup0",
				"DescriptorGroup1",
				"DescriptorGroup2",
				"DescriptorGroup3",
				"DescriptorGroup4",
				"DescriptorGroup5",
				"DescriptorGroup6",
				"DescriptorGroup7",
				"DescriptorGroup8",
				"DescriptorGroup9",
				"DescriptorGroup10",
				"DescriptorGroup11",
				"DescriptorGroup12",
				"DescriptorGroup13",
				"DescriptorGroup14",
				"DescriptorGroup15",

				"None",
				"Input",
				"Output"
			};

			return std::string(names[(uint8_t)specifier]);
		}
	}
}