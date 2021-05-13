#pragma once

#include "Gogaman/Utilities/Algorithm/DirectedGraph.h"

#include "IntermediateRepresentation.h"

namespace Gogaman
{
	namespace FlexShader
	{
		namespace IR
		{
			class SSA
			{
			public:
				SSA(const IR &intermediateRepresentation);
				~SSA() = default;
			private:
				void EliminateRedunantBranches();

				void BuildControlFlowGraph();

				//lhs or rhs of assignment
				template<bool checkAssignment = false>
				bool ContainsVariable(const Instruction &instruction, const Address variable) const
				{
					const auto CheckAddress = [&](const Address address)
					{
						switch(address.GetType())
						{
						case Address::Type::Variable:
							return address == variable;
						case Address::Type::Function:
							return false;
							//May need to handle vectors
						case Address::Type::ConstantOrVector:
							return false;
						case Address::Type::InstructionPointer:
							return ContainsVariable(m_IR.instructions[address.GetValue()], variable);
						}
					};

					//Arithmetic operator
					if((uint8_t)instruction.operation <= 9U)
						return CheckAddress(instruction.address1) || CheckAddress(instruction.address2);

					switch(instruction.operation)
					{
					case Operation::Variable:
					case Operation::VectorSwizzle:
						return instruction.address1 == variable;
					case Operation::Subscript:
						//TODO: Handle this once subscripts implemented
						return false;
					case Operation::Assignment:
							return checkAssignment ? CheckAddress(instruction.address1) : CheckAddress(instruction.address2);
					}
				}
			private:
				IR                                         m_IR;
				DirectedGraph<uint16_t>                    m_ControlFlowGraph;
				std::vector<std::pair<uint32_t, uint32_t>> m_BasicBlocks;
			};
		}
	}
}