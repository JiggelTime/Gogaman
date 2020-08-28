#pragma once

namespace Gogaman
{
	namespace FlexShader
	{
		enum class Type : uint8_t
		{
			None,
			Void,
			Integer,
			Float,
			Float2,
			Float3,
			Float4,
			Sampler
		};

		inline Type GetType(const std::string &typeName)
		{
			if(typeName == "void")
				return Type::Void;
			else if(typeName == "int")
				return Type::Integer;
			else if(typeName == "float")
				return Type::Float;
			else if(typeName == "float2")
				return Type::Float2;
			else if(typeName == "float3")
				return Type::Float3;
			else if(typeName == "float4")
				return Type::Float4;
			else if(typeName == "Sampler")
				return Type::Sampler;
			else
				return Type::None;
		}

		inline std::string GetTypeString(const Type type)
		{
			const std::string names[]
			{
				"none",
				"void",
				"int",
				"float",
				"float2",
				"float3",
				"float4",
				"Sampler"
			};

			return std::string(names[(uint8_t)type]);
		}

		enum class VariableSpecifierFlag : uint8_t
		{
			Input  = 1 << 0,
			Output = 1 << 1
		};

		inline VariableSpecifierFlag GetVariableSpecifierFlag(const std::string &specifierName)
		{
			if(specifierName == "input")
				return VariableSpecifierFlag::Input;
			else if(specifierName == "output")
				return VariableSpecifierFlag::Output;
		}
	}
}