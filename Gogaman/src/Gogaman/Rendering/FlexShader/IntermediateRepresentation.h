#pragma once

#include "Gogaman/Core/Base.h"

#include "Gogaman/Rendering/FlexShader/Type.h"

namespace Gogaman
{
	namespace FlexShader
	{
		namespace IR
		{
			enum class Operation : uint8_t
			{
				//Arithmetic
				Add,
				Subtract,
				Multiply,
				Divide,
				Modulo,

				Assignment,

				//Control flow
				Branch,
				BranchOnNotEqual,

				//Function
				BeginFunction,
				EndFunction,
				Return,
				FunctionParameter,
				FunctionCall
			};

			inline std::string GetOperationString(const Operation operation)
			{
				const std::string names[]
				{
				"Add",
				"Subtract",
				"Multiply",
				"Divide",
				"Modulo",
				"Assignment",
				"Branch",
				"BranchOnNotEqual",
				"BeginFunction",
				"EndFunction",
				"Return",
				"FunctionParameter",
				"FunctionCall"
				};

				return std::string(names[(uint8_t)operation]);
			}

			class Address
			{
			public:
				enum class Type : uint8_t
				{
					Variable,
					Function,
					Constant,
					InstructionPointer
				};

				inline static std::string GetTypeString(const Type type)
				{
					const std::string names[]
					{
					"Variable",
					"Function",
					"Constant",
					"InstructionPointer"
					};

					return std::string(names[(uint8_t)type]);
				}
			public:
				Address(const uint32_t value = 0x3FFFFFFF, const Type type = Type::InstructionPointer)
				{
					GM_DEBUG_ASSERT(value <= 0x3FFFFFFF, "Failed to construct FlexShader intermediate representation address | Invalid value");
					m_Data = value | ((uint32_t)type << 30);
				}

				~Address() = default;

				inline const uint32_t GetValue() const { return m_Data & 0x3fffffff; }

				inline const Type GetType() const { return (Type)(m_Data >> 30); }

				inline const bool IsValid() const { return m_Data != UINT32_MAX; }
			private:
				uint32_t m_Data;
			};

			struct Instruction
			{
				Address   address1, address2;
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

				std::vector<std::pair<Type, uint8_t>>           variableSymbolTable;
				std::unordered_map<uint32_t, FunctionSignature> functionSignatures;
				std::vector<uint32_t>                           constantValues;

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
							GM_LOG_CORE_INFO("%d: %s | Address 1: %d (%s) | Address 2: %d (%s)", executionIndex, GetOperationString(instruction.operation).c_str(), instruction.address1.GetValue(), Address::GetTypeString(instruction.address1.GetType()).c_str(), instruction.address2.GetValue(), Address::GetTypeString(instruction.address2.GetType()).c_str());
						}
						else
							GM_LOG_CORE_INFO("%d: %s | Address 1: %d (%s)", executionIndex, GetOperationString(instruction.operation).c_str(), instruction.address1.GetValue(), Address::GetTypeString(instruction.address1.GetType()).c_str());
					}
					else
						GM_LOG_CORE_INFO("%d: %s", executionIndex, GetOperationString(instruction.operation).c_str());					
				}
			}
		}
	}
}