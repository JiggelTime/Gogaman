#pragma once

#include "Gogaman/Core/Base.h"

#include "Gogaman/Rendering/FlexShader/Type.h"

#define GM_FLEX_SHADER_IR_INVALID_ADDRESS_DATA 3288334335

namespace Gogaman
{
	namespace FlexShader
	{
		namespace IR
		{
			enum class IntrinsicFunction : uint8_t
			{
				None,
				Sin,
				Cos,
				Tan,
				Power,
				NaturalExponentiation,
				NaturalLogarithm,
				SquareRoot,
				InverseSquareRoot,
				Step,
				SmoothStep
			};

			inline IntrinsicFunction GetIntrinsicFunction(const std::string &name)
			{
				if(name == "sin")
					return IntrinsicFunction::Sin;
				else if(name == "cos")
					return IntrinsicFunction::Cos;
				else if(name == "tan")
					return IntrinsicFunction::Tan;
				else if(name == "pow")
					return IntrinsicFunction::Power;
				else if(name == "exp")
					return IntrinsicFunction::NaturalExponentiation;
				else if(name == "log")
					return IntrinsicFunction::NaturalLogarithm;
				else if(name == "sqrt")
					return IntrinsicFunction::SquareRoot;
				else if(name == "inverseSqrt")
					return IntrinsicFunction::InverseSquareRoot;
				else if(name == "step")
					return IntrinsicFunction::Step;
				else if(name == "smoothstep")
					return IntrinsicFunction::SmoothStep;
				else
					return IntrinsicFunction::None;
			}

			enum class Operation : uint8_t
			{
				//Arithmetic
				IntegerAdd,
				IntegerSubtract,
				IntegerMultiply,
				IntegerDivide,
				IntegerModulo,
				FloatAdd,
				FloatSubtract,
				FloatMultiply,
				FloatDivide,
				FloatModulo,

				//Miscellaneous
				Variable,
				Subscript,
				VectorSwizzle,
				Assignment,

				//Control flow
				Branch,
				BranchOnNotEqual,

				//Function
				BeginFunction,
				EndFunction,
				Return,
				FunctionParameter,
				FunctionCall,
				IntrinsicFunctionCall
			};

			inline std::string GetOperationString(const Operation operation)
			{
				const std::string names[]
				{
					"IntegerAdd",
					"IntegerSubtract",
					"IntegerMultiply",
					"IntegerDivide",
					"IntegerModulo",
					"FloatAdd",
					"FloatSubtract",
					"FloatMultiply",
					"FloatDivide",
					"FloatModulo",
					"Variable",
					"Subscript",
					"VectorSwizzle",
					"Assignment",
					"Branch",
					"BranchOnNotEqual",
					"BeginFunction",
					"EndFunction",
					"Return",
					"FunctionParameter",
					"FunctionCall",
					"IntrinsicFunctionCall"
				};

				return std::string(names[(uint8_t)operation]);
			}

			struct HashAddress;

			class Address
			{
			public:
				enum class Type : uint8_t
				{
					Variable,
					Function,
					ConstantOrVector,
					InstructionPointer
				};

				inline static std::string GetTypeString(const Type type)
				{
					const std::string names[]
					{
						"Variable",
						"Function",
						"ConstantOrVector",
						"InstructionPointer"
					};

					return std::string(names[(uint8_t)type]);
				}
			public:
				Address(const Type type = Type::InstructionPointer, const uint32_t value = 0x3FFFFFF, const FlexShader::Type dataType = FlexShader::Type::Void)
				{
					GM_DEBUG_ASSERT(value <= 0x3FFFFFF, "Failed to construct FlexShader intermediate representation address | Invalid value");
					m_Data = ((uint32_t)type << 30) | ((uint32_t)dataType << 26) | value;
				}

				~Address() = default;

				inline bool operator==(const Address &other) const { return m_Data == other.m_Data; }

				inline const Type GetType() const { return (Type)(m_Data >> 30); }

				inline const FlexShader::Type GetDataType() const { return (FlexShader::Type)((m_Data & 0x3FFFFFFF) >> 26); }

				inline const uint32_t GetValue() const { return m_Data & 0x3FFFFFF; }

				inline const bool IsValid() const { return m_Data != GM_FLEX_SHADER_IR_INVALID_ADDRESS_DATA; }
			private:
				uint32_t m_Data;
			private:
				friend HashAddress;
			};

			struct HashAddress
			{
				inline uint32_t operator()(const Address &address) const { return std::hash<uint32_t>()(address.m_Data); }
			};

			struct Instruction
			{
				Instruction(const Operation operation, const Address address1 = {}, const Address address2 = {})
					: operation(operation), address1(address1), address2(address2)
				{}

				Instruction(const Operation operation, const Address address, const uint32_t data)
					: operation(operation), address1(address), data2(data)
				{}

				Instruction(const Operation operation, const uint32_t data)
					: operation(operation), data1(data), address2({})
				{}

				union
				{
					uint32_t data1;
					Address  address1;
				};

				union
				{
					uint32_t data2;
					Address  address2;
				};

				Operation operation;
			};

			struct FunctionSignature
			{
				bool operator==(const FunctionSignature &) const = default;

				std::vector<uint32_t> parameterIDs;
				std::vector<Type>     parameterTypes;
				Type                  returnType;
			};

			struct HashFunctionSignature
			{
				inline uint32_t operator()(const FunctionSignature &signature) const
				{
					uint32_t digest = 0;

					for(const uint32_t i : signature.parameterIDs)
						digest ^= std::hash<uint32_t>()(i) + 0x9e3779b9 + (digest << 6) + (digest >> 2);

					const auto HashType = [&](const Type type) { digest ^= std::hash<uint8_t>()((uint8_t)type) + 0x9e3779b9 + (digest << 6) + (digest >> 2); };

					HashType(signature.returnType);
					for(const Type i : signature.parameterTypes)
						HashType(i);

					return digest;
				};
			};

			struct IR
			{
				std::vector<Instruction> instructions;
				std::vector<uint32_t>    executionOrder;

				std::vector<VariableSpecifier>                  variableSpecifiers;
				std::unordered_map<uint32_t, FunctionSignature> functionSignatures;

				std::vector<uint32_t> integerConstantValues;
				std::vector<float>    floatConstantValues;

				std::vector<Address *> integer2Values;
				std::vector<Address *> integer3Values;
				std::vector<Address *> integer4Values;

				std::vector<Address *> float2Values;
				std::vector<Address *> float3Values;
				std::vector<Address *> float4Values;

				std::vector<Address> descriptors[16];

				//Address entryPoint;
			};

			inline void LogIR(const IR &ir)
			{
				for(auto i = 0; i < ir.executionOrder.size(); i++)
				{
					const uint32_t executionIndex = ir.executionOrder[i];
					const Instruction &instruction = ir.instructions[executionIndex];
					if(instruction.address1.IsValid())
					{
						if(instruction.address2.IsValid())
						{
							GM_LOG_CORE_INFO("%d: %s | Address 1: %d %s (%s) | Address 2: %d %s (%s)", executionIndex, GetOperationString(instruction.operation).c_str(), instruction.address1.GetValue(), GetTypeString(instruction.address1.GetDataType()).c_str(), Address::GetTypeString(instruction.address1.GetType()).c_str(), instruction.address2.GetValue(), GetTypeString(instruction.address2.GetDataType()).c_str(), Address::GetTypeString(instruction.address2.GetType()).c_str());
						}
						else
							GM_LOG_CORE_INFO("%d: %s | Address 1: %d %s (%s)", executionIndex, GetOperationString(instruction.operation).c_str(), instruction.address1.GetValue(), GetTypeString(instruction.address1.GetDataType()).c_str(), Address::GetTypeString(instruction.address1.GetType()).c_str());
					}
					else
						GM_LOG_CORE_INFO("%d: %s", executionIndex, GetOperationString(instruction.operation).c_str());					
				}
			}
		}
	}
}