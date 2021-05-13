#include "pch.h"
#include "StaticSingleAssignment.h"

#define FLEX_SHADER_IS_BRANCH_INSTRUCTION(x) ((x.operation == Operation::Branch) || (x.operation == Operation::BranchOnNotEqual))

namespace Gogaman
{
	namespace FlexShader
	{
		namespace IR
		{
			SSA::SSA(const IR &intermediateRepresentation)
				: m_IR(intermediateRepresentation)
			{
				EliminateRedunantBranches();

				GM_LOG_CORE_INFO("_______________________________________");
				GM_LOG_CORE_INFO("IR after eliminating redundant branches");
				LogIR(m_IR);

				BuildControlFlowGraph();

				//for(auto i = 0; i < 13; i++)
					//m_ControlFlowGraph.CreateVertex();

				//CMPUT 680 example
				//m_ControlFlowGraph.CreateEdge(0, 1);
				//m_ControlFlowGraph.CreateEdge(0, 4);
				//m_ControlFlowGraph.CreateEdge(0, 8);
				//m_ControlFlowGraph.CreateEdge(1, 2);
				//m_ControlFlowGraph.CreateEdge(4, 5);
				//m_ControlFlowGraph.CreateEdge(4, 6);
				//m_ControlFlowGraph.CreateEdge(8, 9);
				//m_ControlFlowGraph.CreateEdge(8, 10);
				//m_ControlFlowGraph.CreateEdge(2, 2);
				//m_ControlFlowGraph.CreateEdge(2, 3);
				//m_ControlFlowGraph.CreateEdge(5, 3);
				//m_ControlFlowGraph.CreateEdge(5, 7);
				//m_ControlFlowGraph.CreateEdge(6, 7);
				//m_ControlFlowGraph.CreateEdge(6, 11);
				//m_ControlFlowGraph.CreateEdge(9, 11);
				//m_ControlFlowGraph.CreateEdge(10, 11);
				//m_ControlFlowGraph.CreateEdge(11, 12);
				//m_ControlFlowGraph.CreateEdge(7, 4);
				//m_ControlFlowGraph.CreateEdge(7, 12);
				//m_ControlFlowGraph.CreateEdge(3, 12);

				//Figure 4 graph
				//m_ControlFlowGraph.CreateEdge(0, 4);
				//m_ControlFlowGraph.CreateEdge(0, 3);
				//m_ControlFlowGraph.CreateEdge(4, 5);
				//m_ControlFlowGraph.CreateEdge(3, 2);
				//m_ControlFlowGraph.CreateEdge(3, 1);
				//m_ControlFlowGraph.CreateEdge(1, 2);
				//m_ControlFlowGraph.CreateEdge(2, 1);
				//m_ControlFlowGraph.CreateEdge(5, 1);
				//m_ControlFlowGraph.CreateEdge(1, 5);

				//Build flattened dominator tree
				const std::vector<uint16_t> postOrder = m_ControlFlowGraph.PostOrder(0);
				for(auto i : postOrder)
					printf("%d\n", i);

				std::vector<uint16_t> postOrderIndices;
				postOrderIndices.resize(postOrder.size());
				for(auto i = 0; i < postOrder.size(); i++)
					postOrderIndices[postOrder[i]] = i;

				constexpr uint16_t undefinedDominator = UINT16_MAX;
				std::vector<uint16_t> dominators(m_ControlFlowGraph.GetVertexCount(), undefinedDominator);

				dominators[postOrderIndices[0]] = postOrderIndices[0];
				bool changed = true;
				while(changed)
				{
					changed = false;
					for(auto i = postOrder.rbegin() + 1; i != postOrder.rend(); i++)
					{
						const std::vector<uint16_t> predecessors = m_ControlFlowGraph.IncomingConnections(*i);

						uint16_t firstProcessedPredecessorIndex;
						for(firstProcessedPredecessorIndex = 0; firstProcessedPredecessorIndex < predecessors.size(); firstProcessedPredecessorIndex++)
							if(dominators[postOrderIndices[predecessors[firstProcessedPredecessorIndex]]] != undefinedDominator)
								break;

						uint16_t newImmediateDominator = postOrderIndices[predecessors[firstProcessedPredecessorIndex]];
						for(uint16_t j = firstProcessedPredecessorIndex + 1; j < predecessors.size(); j++)
							if(dominators[postOrderIndices[predecessors[j]]] != undefinedDominator)
							{
								uint16_t index1 = postOrderIndices[predecessors[j]];
								uint16_t index2 = newImmediateDominator;
								while(index1 != index2)
								{
									while(index1 < index2)
										index1 = dominators[index1];
									while(index2 < index1)
										index2 = dominators[index2];
								}

								newImmediateDominator = index1;
							}

						if(dominators[postOrderIndices[*i]] != newImmediateDominator)
						{
							dominators[postOrderIndices[*i]] = newImmediateDominator;
							changed = true;
						}
					}
				}

				for(auto i = 0; i < dominators.size(); i++)
					GM_LOG_CORE_INFO("Immediate dominator of node %d: %d", postOrder[i], postOrder[dominators[i]]);

				//Calculate dominance frontiers
				std::vector<std::set<uint16_t>> dominanceFrontiers;
				dominanceFrontiers.resize(m_ControlFlowGraph.GetVertexCount());
				for(uint16_t i = 0; i < m_ControlFlowGraph.GetVertexCount(); i++)
				{
					const std::vector<uint16_t> predecessors = m_ControlFlowGraph.IncomingConnections(i);

					if(predecessors.size() > 1)
						for(const uint16_t j : predecessors)
						{
							uint16_t runner = postOrderIndices[j];
							//while(runner != dominators[i])
							while(runner != dominators[postOrderIndices[i]])
							{
								dominanceFrontiers[runner].emplace(i);
								runner = dominators[runner];
							}
						}
				}

				for(auto i = 0; i < dominanceFrontiers.size(); i++)
				{
					GM_LOG_CORE_INFO("Dominance frontiers of node %d:", postOrder[i]);
					for(auto j : dominanceFrontiers[i])
						GM_LOG_CORE_INFO("	%d", j);
				}

				/*
				Rebuild IR in SSA form (with phi-functions and variable version declaration instructions inserted)
				-Can we do it simultaniously?
				*/

				//Mapping from versionedID -> originalID 
				std::unordered_map<Address, Address, HashAddress> originalVariableAddresses;
				/*
				for(uint16_t i = 0; i < m_BasicBlocks.size(); i++)
				{
					auto &[startInstructionIndex, endInstructionIndex] = m_BasicBlocks[i];

					uint32_t currentInstructionIndex = startInstructionIndex;
					while(currentInstructionIndex != endInstructionIndex)
					{
						Instruction &currentInstruction = m_IR.instructions[currentInstructionIndex];
						if(currentInstruction.operation == Operation::Assignment)
						{
							const Address originalAddress = currentInstruction.address1;
							//New variable version (might need to insert variable declaration instruction...)
							currentInstruction.address1 = Address(Address::Type::Variable, m_IR.variableSpecifiers.size(), currentInstruction.address1.GetDataType());
							//Copy variable specifier
							m_IR.variableSpecifiers.emplace_back(m_IR.variableSpecifiers[originalAddress.GetValue()]);

							originalVariableAddresses[currentInstruction.address1] = originalAddress;
						}

						currentInstructionIndex++;
					}
				}
				*/

				std::vector<std::vector<Address>> blockPhiFunctions(m_BasicBlocks.size());
				for(uint16_t i = 0; i < m_BasicBlocks.size(); i++)
				{
					auto &[startInstructionIndex, endInstructionIndex] = m_BasicBlocks[i];
					uint32_t currentInstructionIndex = startInstructionIndex;
					while(currentInstructionIndex != endInstructionIndex + 1)
					{
						Instruction &currentInstruction = m_IR.instructions[currentInstructionIndex];
						if(currentInstruction.operation == Operation::Assignment)
						{
							//Assumption: LHS of assignment is always either variable address, variable declaration instruction, or swizzle instruction
							GM_DEBUG_ASSERT((currentInstruction.address1.GetType() == Address::Type::Variable) || (currentInstruction.address1.GetType() == Address::Type::InstructionPointer), "Failed to generate FlexShader SSA phi functions | Invalid assignment instruction");
							Address definedVariable = (currentInstruction.address1.GetType() == Address::Type::Variable) ? currentInstruction.address1 : m_IR.instructions[currentInstruction.address1.GetValue()].address1;

							const auto IsBlockUsingVariable = [&](const uint16_t blockIndex, const Address address)
							{
								for(uint32_t j = m_BasicBlocks[blockIndex].first; j != m_BasicBlocks[blockIndex].second; j++)
									if((m_IR.instructions[j].operation == Operation::Assignment) || (m_IR.instructions[j].operation == Operation::FunctionParameter))
										if(ContainsVariable(m_IR.instructions[j], address))
											return true;

								return false;
							};

							for(const uint16_t j : dominanceFrontiers[postOrderIndices[i]])
								if(IsBlockUsingVariable(j, definedVariable))
									blockPhiFunctions[j].emplace_back(definedVariable);
						}

						currentInstructionIndex++;
					}
				}

				for(uint16_t i = 0; i < blockPhiFunctions.size(); i++)
					for(uint16_t j = 0; j < blockPhiFunctions[i].size(); j++)
						GM_LOG_CORE_INFO("Block %d | Phi arg %d: %d %s (%s)", i, j, blockPhiFunctions[i][j].GetValue(), GetTypeString(blockPhiFunctions[i][j].GetDataType()).c_str(), Address::GetTypeString(blockPhiFunctions[i][j].GetType()).c_str());
			}

			void SSA::EliminateRedunantBranches()
			{
				for(uint32_t i = 0; i < m_IR.executionOrder.size(); i++)
				{
					const uint32_t instructionIndex = m_IR.executionOrder[i];
					auto &instruction = m_IR.instructions[instructionIndex];

					if(instruction.operation == Operation::Branch)
					{
						const auto &targetInstruction = m_IR.instructions[instruction.address1.GetValue()];
						if(targetInstruction.operation == Operation::Branch)
							instruction.address1 = targetInstruction.address1;
					}
				}
			}

			void SSA::BuildControlFlowGraph()
			{
				std::unordered_map<uint32_t, uint32_t> instructionExecutionIndices;
				instructionExecutionIndices.reserve(m_IR.executionOrder.size());
				for(uint32_t i = 0; i < m_IR.executionOrder.size(); i++)
					instructionExecutionIndices[m_IR.executionOrder[i]] = i;

				const auto IsBranching = [](const Instruction &instruction) { return (instruction.operation == Operation::Branch) || (instruction.operation == Operation::BranchOnNotEqual); };

				//Generate leaders
				std::set<uint32_t> leaders;
				leaders.emplace(0);
				bool previousInstructionWasBranch = IsBranching(m_IR.instructions[m_IR.executionOrder[0]]);
				for(auto i = 1; i < m_IR.executionOrder.size(); i++)
				{
					if(previousInstructionWasBranch)
						leaders.emplace(i);

					const Instruction &instruction = m_IR.instructions[m_IR.executionOrder[i]];
					if(IsBranching(instruction))
					{
						if(instruction.operation == Operation::Branch)
							leaders.emplace(instructionExecutionIndices[instruction.address1.GetValue()]);
						else if(instruction.operation == Operation::BranchOnNotEqual)
							leaders.emplace(instructionExecutionIndices[instruction.address2.GetValue()]);
						
						previousInstructionWasBranch = true;
					}
					else
						previousInstructionWasBranch = false;
				}

				//Generate basic blocks
				std::unordered_map<uint32_t, uint32_t> leaderBlockIndices;
				leaderBlockIndices.reserve(leaders.size());
				while(!leaders.empty())
				{
					const uint32_t blockStart = *leaders.begin();
					leaders.erase(leaders.begin());

					uint32_t blockEnd = blockStart;
					for(uint32_t i = blockStart + 1; (i < m_IR.executionOrder.size()) && !leaders.contains(i); i++)
						blockEnd = i;

					leaderBlockIndices[blockStart] = m_BasicBlocks.size();
					m_BasicBlocks.emplace_back(blockStart, blockEnd);
					m_ControlFlowGraph.CreateVertex();
				}

				//Create graph edges
				for(uint16_t i = 0; i < m_BasicBlocks.size(); i++)
				{
					const uint32_t endInstructionExecutionIndex = m_IR.executionOrder[m_BasicBlocks[i].second];
					const Instruction &endInstruction = m_IR.instructions[endInstructionExecutionIndex];
					if(endInstruction.operation == Operation::Branch)
						m_ControlFlowGraph.CreateEdge(i, leaderBlockIndices[instructionExecutionIndices[endInstruction.address1.GetValue()]]);
					else if(endInstruction.operation == Operation::BranchOnNotEqual)
					{
						m_ControlFlowGraph.CreateEdge(i, leaderBlockIndices[instructionExecutionIndices[endInstruction.address2.GetValue()]]);
						if(i + 1 < m_BasicBlocks.size())
							m_ControlFlowGraph.CreateEdge(i, i + 1);
					}
					else if((endInstructionExecutionIndex + 1) != m_IR.executionOrder.size())
						m_ControlFlowGraph.CreateEdge(i, leaderBlockIndices[endInstructionExecutionIndex + 1]);
				}
			}
		}
	}
}