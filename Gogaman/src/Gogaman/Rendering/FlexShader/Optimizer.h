#pragma once

#include "IntermediateRepresentation.h"

namespace Gogaman
{
	namespace FlexShader
	{
		namespace IR
		{
			void Optimize(const IR &ir)
			{
				IR newIr;
				newIr.variableSpecifiers = ir.variableSpecifiers;
				newIr.functionSignatures = ir.functionSignatures;

				newIr.integerConstantValues = ir.integerConstantValues;
				newIr.floatConstantValues   = ir.floatConstantValues;

				//std::unordered_map<Address, Address> resolvedConstantInstructions;

				//const auto GetValue = [&](const Address &address)
				//{
				//	if(address.GetDataType() == Type::Integer)
				//		return newIr.integerConstantValues[address.GetValue()];
				//	else if(address.GetDataType() == Type::Float)
				//		return newIr.floatConstantValues[address.GetValue()];
				//
				//};

				for(unsigned int i = 0; i < ir.executionOrder.size(); i++)
				{
					const uint32_t instructionIndex = ir.executionOrder[i];
					const Instruction &instruction = ir.instructions[instructionIndex];

					if(instruction.operation <= Operation::FloatModulo)
					{
						if((instruction.address1.GetType() == Address::Type::ConstantOrVector) && (instruction.address2.GetType() == Address::Type::ConstantOrVector))
						{
							//const Type resultType = instruction.address1.GetDataType();

							switch(instruction.operation)
							{
							case Operation::IntegerAdd:
							case Operation::IntegerSubtract:
							case Operation::IntegerMultiply:
							case Operation::IntegerDivide:
							case Operation::IntegerModulo:
							case Operation::FloatAdd:
							case Operation::FloatSubtract:
							case Operation::FloatMultiply:
							case Operation::FloatDivide:
							case Operation::FloatModulo:
								break;
							}

							//resolvedConstantInstructions[{ Address::Type::InstructionPointer, instructionIndex }] = { Address::Type::ConstantOrVector, fmod(newIr.floatConstantValues[instruction.address1.GetValue()], newIr.floatConstantValues[instruction.address2.GetValue()]), Type::Float };
						}
					}
				}
				/*
				for(auto &[inst, rslt] : resolvedConstantInstructions)
					if(rslt.GetDataType() == Type::Integer)
					{
						GM_LOG_CORE_INFO("Instruction %d resolved to value %d", inst.GetValue(), rslt.GetValue());
					}
					else if(rslt.GetDataType() == Type::Float)
						GM_LOG_CORE_INFO("Instruction %d resolved to value %f", inst.GetValue(), rslt.GetValue());*/
				//LogIR(newIr);
			}
		}
	}
}
