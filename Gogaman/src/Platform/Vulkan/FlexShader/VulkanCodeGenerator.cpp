#include "pch.h"
#include "VulkanCodeGenerator.h"

#include "Gogaman/Utilities/Cast.h"

#include "Gogaman/Rendering/FlexShader/IntermediateRepresentation.h"

namespace Gogaman
{
	namespace FlexShader
	{
		CodeGenerator::CodeGenerator(const IR::IR &intermediateRepresentation, const std::string &entryPointName)
			: AbstractCodeGenerator(intermediateRepresentation, entryPointName)
		{
			using namespace IR;

			uint32_t currentID = 1;

			const uint32_t glslStd450InstructionSetID = currentID++;

			//OpTypeVoid
			m_TypeDeclarationInstructions.AddInstruction(19, currentID);
			m_TypeIDs[(uint8_t)Type::Void] = currentID++;
			//OpTypeBool
			m_TypeDeclarationInstructions.AddInstruction(20, currentID);
			m_TypeIDs[(uint8_t)Type::Boolean] = currentID++;
			//OpTypeInt
			m_TypeDeclarationInstructions.AddInstruction(21, currentID, 32, 1);
			m_TypeIDs[(uint8_t)Type::Integer] = currentID++;
			//OpTypeVector | Component count: 2
			m_TypeDeclarationInstructions.AddInstruction(23, currentID, m_TypeIDs[(uint8_t)Type::Integer], 2);
			m_TypeIDs[(uint8_t)Type::Integer2] = currentID++;
			//OpTypeVector | Component count: 3
			m_TypeDeclarationInstructions.AddInstruction(23, currentID, m_TypeIDs[(uint8_t)Type::Integer], 3);
			m_TypeIDs[(uint8_t)Type::Integer3] = currentID++;
			//OpTypeVector | Component count: 4
			m_TypeDeclarationInstructions.AddInstruction(23, currentID, m_TypeIDs[(uint8_t)Type::Integer], 4);
			m_TypeIDs[(uint8_t)Type::Integer4] = currentID++;
			//OpTypeFloat | 32-bit
			m_TypeDeclarationInstructions.AddInstruction(22, currentID, 32);
			m_TypeIDs[(uint8_t)Type::Float] = currentID++;
			//OpTypeVector | Component count: 2
			m_TypeDeclarationInstructions.AddInstruction(23, currentID, m_TypeIDs[(uint8_t)Type::Float], 2);
			m_TypeIDs[(uint8_t)Type::Float2] = currentID++;
			//OpTypeVector | Component count: 3
			m_TypeDeclarationInstructions.AddInstruction(23, currentID, m_TypeIDs[(uint8_t)Type::Float], 3);
			m_TypeIDs[(uint8_t)Type::Float3] = currentID++;
			//OpTypeVector | Component count: 4
			m_TypeDeclarationInstructions.AddInstruction(23, currentID, m_TypeIDs[(uint8_t)Type::Float], 4);
			m_TypeIDs[(uint8_t)Type::Float4] = currentID++;
			//OpTypeMatrix | Column count: 2
			m_TypeDeclarationInstructions.AddInstruction(24, currentID, m_TypeIDs[(uint8_t)Type::Float2], 2);
			m_TypeIDs[(uint8_t)Type::Float2x2] = currentID++;
			//OpTypeMatrix | Column count: 3
			m_TypeDeclarationInstructions.AddInstruction(24, currentID, m_TypeIDs[(uint8_t)Type::Float3], 3);
			m_TypeIDs[(uint8_t)Type::Float3x3] = currentID++;
			//OpTypeMatrix | Column count: 4
			m_TypeDeclarationInstructions.AddInstruction(24, currentID, m_TypeIDs[(uint8_t)Type::Float4], 4);
			m_TypeIDs[(uint8_t)Type::Float4x4] = currentID++;

			for(uint8_t i = 1; i < (uint8_t)Type::Count - 1; i++)
			{
				//OpTypePointer | Storage class: Function
				m_TypeDeclarationInstructions.AddInstruction(32, currentID, 7, m_TypeIDs[i]);
				m_FunctionVariableTypeIDs[i] = currentID++;

				//OpTypePointer | Storage class: Input
				m_TypeDeclarationInstructions.AddInstruction(32, currentID, 1, m_TypeIDs[i]);
				m_InputVariableTypeIDs[i] = currentID++;

				//OpTypePointer | Storage class: Output
				m_TypeDeclarationInstructions.AddInstruction(32, currentID, 3, m_TypeIDs[i]);
				m_OutputVariableTypeIDs[i] = currentID++;
			}

			const uint32_t descriptorTypeID = currentID++;

			struct HashStructType
			{
				uint32_t operator()(const std::vector<Type> &types) const
				{
					uint32_t digest = 0;

					for(const Type i : types)
						digest ^= std::hash<uint8_t>()((uint8_t)i) + 0x9e3779b9 + (digest << 6) + (digest >> 2);

					return digest;
				}
			};

			std::unordered_map<std::vector<Type>, uint32_t, HashStructType> structTypeIDs;
			structTypeIDs.reserve(16);
			uint32_t descriptorGroupTypeIDs[16];

			//We need the index of each member in the group. Maybe get offset using difference between the base address? Needed for decorations later too...

			for(uint8_t i = 0; i < 16; i++)
			{
				if(intermediateRepresentation.descriptors[i].size())
				{
					std::vector<Type> types;
					types.reserve(intermediateRepresentation.descriptors[i].size());
					for(const Address j : intermediateRepresentation.descriptors[i])
						types.emplace_back(j.GetDataType());

					if(!structTypeIDs.contains(types))
					{
						structTypeIDs[types] = i == 0 ? currentID : ++currentID;
						currentID++;
					}

					descriptorGroupTypeIDs[i] = currentID;
				}
			}

			currentID++;

			std::unordered_set<FunctionSignature, HashFunctionSignature>           functionSignatures;
			//This hash function uses parameter IDs and parameter types... it should only be the type probably
			std::unordered_map<FunctionSignature, uint32_t, HashFunctionSignature> functionSignatureTypeIDs;
			for(auto &[functionID, signature] : intermediateRepresentation.functionSignatures)
			{
				if(!functionSignatures.contains(signature))
				{
					functionSignatureTypeIDs[signature] = currentID++;
					functionSignatures.insert(signature);
				}
			}

			std::unordered_map<uint32_t, uint32_t> leaderInstructionBasicBlocks;
			for(uint32_t i = 0; i < intermediateRepresentation.executionOrder.size(); i++)
			{
				const Instruction &instruction = intermediateRepresentation.instructions[intermediateRepresentation.executionOrder[i]];
				switch(instruction.operation)
				{
				case Operation::Branch:
					if(!leaderInstructionBasicBlocks.contains(instruction.address1.GetValue()))
						leaderInstructionBasicBlocks[instruction.address1.GetValue()] = currentID++;

					if((i + 1) < intermediateRepresentation.executionOrder.size())
					{
						const uint32_t nextInstructionIndex = intermediateRepresentation.executionOrder[i + 1];
						if(!leaderInstructionBasicBlocks.contains(nextInstructionIndex))
							leaderInstructionBasicBlocks[nextInstructionIndex] = currentID++;
					}

					break;
				case Operation::BranchOnNotEqual:
					if(!leaderInstructionBasicBlocks.contains(instruction.address2.GetValue()))
						leaderInstructionBasicBlocks[instruction.address2.GetValue()] = currentID++;

					if((i + 1) < intermediateRepresentation.executionOrder.size())
					{
						const uint32_t nextInstructionIndex = intermediateRepresentation.executionOrder[i + 1];
						if(!leaderInstructionBasicBlocks.contains(nextInstructionIndex))
							leaderInstructionBasicBlocks[nextInstructionIndex] = currentID++;
					}

					break;
				}
			}

			std::unordered_map<uint32_t, uint32_t> variableIDs;
			variableIDs.reserve(intermediateRepresentation.variableSpecifiers.size());
			std::vector<uint32_t> functionIDs, instructionResultIDs;
			functionIDs.reserve(intermediateRepresentation.functionSignatures.size());
			instructionResultIDs.resize(intermediateRepresentation.instructions.size());

			//OpConstantFalse
			m_ConstantInstructions.AddInstruction(42, m_TypeIDs[(uint8_t)Type::Boolean], currentID);
			m_BooleanConstantIDs[0] = currentID++;
			//OpConstantTrue
			m_ConstantInstructions.AddInstruction(41, m_TypeIDs[(uint8_t)Type::Boolean], currentID);
			m_BooleanConstantIDs[1] = currentID++;

			m_IntegerConstantIDs.reserve(intermediateRepresentation.integerConstantValues.size());
			for(uint32_t i = 0; i < intermediateRepresentation.integerConstantValues.size(); i++)
			{
				//OpConstant
				m_ConstantInstructions.AddInstruction(43, m_TypeIDs[(uint8_t)Type::Integer], currentID, intermediateRepresentation.integerConstantValues[i]);
				m_IntegerConstantIDs.emplace_back(currentID++);
			}

			m_FloatConstantIDs.reserve(intermediateRepresentation.floatConstantValues.size());
			for(uint32_t i = 0; i < intermediateRepresentation.floatConstantValues.size(); i++)
			{
				GM_STATIC_ASSERT(sizeof(float) == sizeof(uint32_t));
				//OpConstant
				m_ConstantInstructions.AddInstruction(43, m_TypeIDs[(uint8_t)Type::Float], currentID, BitwiseCast<uint32_t>(intermediateRepresentation.floatConstantValues[i]));
				m_FloatConstantIDs.emplace_back(currentID++);
			}

			std::vector<uint32_t> inputIDs, outputIDs;

			uint8_t largestDescriptorGroupDescriptorCount = 0;
			uint32_t descriptorGroupIDs[16];
			Address descriptorGroupBaseAddresses[16];
			for(uint8_t i = 0; i < 16; i++)
			{
				const uint8_t descriptorCount = intermediateRepresentation.descriptors[i].size();
				if(descriptorCount)
				{
					if(descriptorCount > largestDescriptorGroupDescriptorCount)
						largestDescriptorGroupDescriptorCount = descriptorCount;

					descriptorGroupIDs[i] = currentID++;
					descriptorGroupBaseAddresses[i] = intermediateRepresentation.descriptors[i].front();
				}
			}

			std::vector<uint32_t> structMemberAccessConstantIDs;
			structMemberAccessConstantIDs.reserve(std::max((uint32_t)largestDescriptorGroupDescriptorCount, 4u));
			for(uint8_t i = 0; i < std::max((uint32_t)largestDescriptorGroupDescriptorCount, 4u); i++)
				structMemberAccessConstantIDs.emplace_back(currentID++);

			InstructionStream irStream;

			const auto GetAddressID = [&](const Address address, const bool loadVariable = false)
			{
				switch(address.GetType())
				{
				case Address::Type::Variable:
					if(!variableIDs.contains(address.GetValue()))
					{
						const VariableSpecifier specifier = intermediateRepresentation.variableSpecifiers[address.GetValue()];
						if((uint8_t)specifier < 16)
						{
							const uint32_t identifier = currentID++;
							variableIDs[address.GetValue()] = identifier;

							const uint32_t offset = address.GetValue() - descriptorGroupBaseAddresses[(uint8_t)specifier].GetValue();
							//OpAccessChain
							irStream.AddInstruction(65, descriptorTypeID, identifier, descriptorGroupIDs[(uint8_t)specifier], structMemberAccessConstantIDs[offset]);
						}
					}

					if(loadVariable)
					{
						//OpLoad
						irStream.AddInstruction(61, m_TypeIDs[(uint8_t)address.GetDataType()], currentID, variableIDs[address.GetValue()]);
						return currentID++;
					}
					else
						return variableIDs[address.GetValue()];
				case Address::Type::Function:
					return functionIDs[address.GetValue()];
				case Address::Type::ConstantOrVector:
				{
					switch(address.GetDataType())
					{
					case Type::Boolean:
						return m_BooleanConstantIDs[address.GetValue()];
					case Type::Integer:
						return m_IntegerConstantIDs[address.GetValue()];
					case Type::Float:
						return m_FloatConstantIDs[address.GetValue()];
					}
				}
				case Address::Type::InstructionPointer:
				{
					if(loadVariable)
					{
						const Instruction &intr = intermediateRepresentation.instructions[address.GetValue()];
						if(intr.operation == Operation::VectorSwizzle)
						{
							if(GetTypeComponentCount((Type)(intr.data2 & 0xFF)) == 1)
							{
								//OpLoad
								irStream.AddInstruction(61, m_TypeIDs[intr.data2 & 0xFF], currentID, instructionResultIDs[address.GetValue()]);
								return currentID++;
							}
						}
					}

					return instructionResultIDs[address.GetValue()];
				}
				}
			};

			for(uint32_t i = 0; i < intermediateRepresentation.executionOrder.size(); i++)
			{
				const uint32_t instructionIndex = intermediateRepresentation.executionOrder[i];

				if(leaderInstructionBasicBlocks.contains(instructionIndex))
					//OpLabel
					irStream.AddInstruction(248, leaderInstructionBasicBlocks[instructionIndex]);

				//NOT EVERY INSTRUCTION HAS A RESULT
				instructionResultIDs[i] = currentID++;

				Instruction instruction = intermediateRepresentation.instructions[instructionIndex];
				switch(instruction.operation)
				{
				case Operation::IntegerAdd:
					//OpFAdd
					irStream.AddInstruction(128, m_TypeIDs[(uint8_t)instruction.address1.GetDataType()], instructionResultIDs[i], GetAddressID(instruction.address1, true), GetAddressID(instruction.address2, true));
					break;
				case Operation::IntegerSubtract:
					//OpFSub
					irStream.AddInstruction(130, m_TypeIDs[(uint8_t)instruction.address1.GetDataType()],  instructionResultIDs[i], GetAddressID(instruction.address1, true), GetAddressID(instruction.address2, true));
					break;
				case Operation::IntegerMultiply:
					//OpFMul
					irStream.AddInstruction(132, m_TypeIDs[(uint8_t)instruction.address1.GetDataType()], instructionResultIDs[i], GetAddressID(instruction.address1, true), GetAddressID(instruction.address2, true));
					break;
				case Operation::IntegerDivide:
					//OpFDiv
					irStream.AddInstruction(135, m_TypeIDs[(uint8_t)instruction.address1.GetDataType()], instructionResultIDs[i], GetAddressID(instruction.address1, true), GetAddressID(instruction.address2, true));
					break;
				case Operation::IntegerModulo:
					//OpFMod
					irStream.AddInstruction(139, m_TypeIDs[(uint8_t)instruction.address1.GetDataType()], instructionResultIDs[i], GetAddressID(instruction.address1, true), GetAddressID(instruction.address2, true));
					break;
				case Operation::FloatAdd:
					//OpFAdd
					irStream.AddInstruction(129, m_TypeIDs[(uint8_t)instruction.address1.GetDataType()], instructionResultIDs[i], GetAddressID(instruction.address1, true), GetAddressID(instruction.address2, true));
					break;
				case Operation::FloatSubtract:
					//OpFSub
					irStream.AddInstruction(131, m_TypeIDs[(uint8_t)instruction.address1.GetDataType()],  instructionResultIDs[i], GetAddressID(instruction.address1, true), GetAddressID(instruction.address2, true));
					break;
				case Operation::FloatMultiply:
					//OpFMul
					irStream.AddInstruction(133, m_TypeIDs[(uint8_t)instruction.address1.GetDataType()], instructionResultIDs[i], GetAddressID(instruction.address1, true), GetAddressID(instruction.address2, true));
					break;
				case Operation::FloatDivide:
					//OpFDiv
					irStream.AddInstruction(136, m_TypeIDs[(uint8_t)instruction.address1.GetDataType()], instructionResultIDs[i], GetAddressID(instruction.address1, true), GetAddressID(instruction.address2, true));
					break;
				case Operation::FloatModulo:
					//OpFMod
					irStream.AddInstruction(141, m_TypeIDs[(uint8_t)instruction.address1.GetDataType()], instructionResultIDs[i], GetAddressID(instruction.address1, true), GetAddressID(instruction.address2, true));
					break;
				case Operation::VectorSwizzle:
				{
					const Type    resultType     = (Type)(instruction.data2 & 0xFF);
					const uint8_t componentCount = GetTypeComponentCount(resultType);
					if(componentCount == 1)
					{
						if(intermediateRepresentation.variableSpecifiers[instruction.address1.GetValue()] == VariableSpecifier::None)
							//OpAccessChain
							irStream.AddInstruction(65, m_FunctionVariableTypeIDs[(uint8_t)resultType], instructionResultIDs[i], GetAddressID(instruction.address1), structMemberAccessConstantIDs[(instruction.data2 >> 8) & 3]);
						else if(intermediateRepresentation.variableSpecifiers[instruction.address1.GetValue()] == VariableSpecifier::Input)
							//OpAccessChain
							irStream.AddInstruction(65, m_InputVariableTypeIDs[(uint8_t)resultType], instructionResultIDs[i], GetAddressID(instruction.address1), structMemberAccessConstantIDs[(instruction.data2 >> 8) & 3]);
						else if(intermediateRepresentation.variableSpecifiers[instruction.address1.GetValue()] == VariableSpecifier::Output)
							//OpAccessChain
							irStream.AddInstruction(65, m_OutputVariableTypeIDs[(uint8_t)resultType], instructionResultIDs[i], GetAddressID(instruction.address1), structMemberAccessConstantIDs[(instruction.data2 >> 8) & 3]);
						else
							//OpAccessChain
							irStream.AddInstruction(65, descriptorTypeID, instructionResultIDs[i], GetAddressID(instruction.address1), structMemberAccessConstantIDs[(instruction.data2 >> 8) & 3]);
					}
					else
					{
						std::vector<uint32_t> components;
						components.reserve(componentCount);
						for(uint8_t i = 0; i < componentCount; i++)
							components.emplace_back((instruction.data2 >> (8 + i * 2)) & 3);

						const uint32_t vectorAddress = GetAddressID(instruction.address1, true);
						//OpVectorShuffle
						irStream.AddInstruction(79, m_TypeIDs[(uint8_t)resultType], instructionResultIDs[i], vectorAddress, vectorAddress, components);
					}
					break;
				}
				case Operation::Assignment:
					//OpStore
					irStream.AddInstruction(62, GetAddressID(instruction.address1), GetAddressID(instruction.address2, true));
					break;
				case Operation::Branch:
					//OpBranch
					irStream.AddInstruction(249, leaderInstructionBasicBlocks[instruction.address1.GetValue()]);
					break;
				case Operation::BranchOnNotEqual:
					//OpBranchConditional
					irStream.AddInstruction(250, GetAddressID(instruction.address1, true), { leaderInstructionBasicBlocks[intermediateRepresentation.executionOrder[i + 1]], leaderInstructionBasicBlocks[instruction.address2.GetValue()] });
					break;
				case Operation::BeginFunction:
				{
					const FunctionSignature &signature = intermediateRepresentation.functionSignatures.at(instruction.address1.GetValue());
					//OpFunction
					irStream.AddInstruction(54, m_TypeIDs[(uint8_t)signature.returnType], functionIDs.emplace_back(currentID++), 0, functionSignatureTypeIDs[signature]);
					for(auto i = 0; i < signature.parameterIDs.size(); i++)
					{
						variableIDs[signature.parameterIDs[i]] = currentID++;
						//OpFunctionParameter
						irStream.AddInstruction(55, m_FunctionVariableTypeIDs[(uint8_t)signature.parameterTypes[i]], { variableIDs[signature.parameterIDs[i]] });
					}

					//OpLabel
					irStream.AddInstruction(248, currentID++);

					uint32_t executionIndex = i + 1;
					Instruction currentInstruction = intermediateRepresentation.instructions[intermediateRepresentation.executionOrder[executionIndex]];
					while(currentInstruction.operation != Operation::EndFunction)
					{
						if(currentInstruction.operation == Operation::Variable)
						{
							variableIDs[currentInstruction.address1.GetValue()] = currentID;
							if(intermediateRepresentation.variableSpecifiers[currentInstruction.address1.GetValue()] == VariableSpecifier::Input)
							{
								//OpDecorate | Location
								m_DecorationInstructions.AddInstruction(71, currentID, 30, inputIDs.size());
								//OpVariable | Storage class: Input
								m_GlobalVariableDeclarationInstructions.AddInstruction(59, m_InputVariableTypeIDs[(uint8_t)currentInstruction.address1.GetDataType()], currentID, 1);
								inputIDs.emplace_back(currentID);
							}
							else if(intermediateRepresentation.variableSpecifiers[currentInstruction.address1.GetValue()] == VariableSpecifier::Output)
							{
								//OpDecorate | Location
								m_DecorationInstructions.AddInstruction(71, currentID, 30, outputIDs.size());
								//OpVariable | Storage class: Output
								m_GlobalVariableDeclarationInstructions.AddInstruction(59, m_OutputVariableTypeIDs[(uint8_t)currentInstruction.address1.GetDataType()], currentID, 3);
								outputIDs.emplace_back(currentID);
							}
							else
								//OpVariable | Storage class: Function
								irStream.AddInstruction(59, m_FunctionVariableTypeIDs[(uint8_t)currentInstruction.address1.GetDataType()], currentID, 7);
							currentID++;
						}

						currentInstruction = intermediateRepresentation.instructions[intermediateRepresentation.executionOrder[++executionIndex]];
					}

					break;
				}
				case Operation::EndFunction:
					//Inject return
					//OpReturn
					//irStream.AddInstruction(253);
					//OpFunctionEnd
					irStream.AddInstruction(56);

					break;
				case Operation::Return:
					if(instruction.address1.IsValid())
						//OpReturnValue
						irStream.AddInstruction(254, GetAddressID(instruction.address1, true));
					else
						//OpReturn
						irStream.AddInstruction(253);
					break;
				case Operation::FunctionParameter:
				{
					std::vector<uint32_t> argumentIDs;
					#if 0
					while(instruction.operation == Operation::FunctionParameter)
					{
						if(instruction.address1.GetType() == Address::Type::ConstantOrVector)
						{
							//OpVariable | Storage class: Function
							irStream.AddInstruction(59, m_FunctionVariableTypeIDs[(uint8_t)instruction.address1.GetDataType()], currentID, 7, GetAddressID(instruction.address1));
							argumentIDs.emplace_back(currentID++);
						}
						else
							argumentIDs.emplace_back(GetAddressID(instruction.address1));

						instruction = intermediateRepresentation.instructions[intermediateRepresentation.executionOrder[++i]];
						instructionResultIDs[i] = currentID++;
					}
					#endif
					while(instruction.operation == Operation::FunctionParameter)
					{
						argumentIDs.emplace_back(GetAddressID(instruction.address1, true));

						instruction = intermediateRepresentation.instructions[intermediateRepresentation.executionOrder[++i]];
						instructionResultIDs[i] = currentID++;
					}

					if(instruction.operation == Operation::FunctionCall)
						//OpFunctionCall
						irStream.AddInstruction(57, m_TypeIDs[(uint8_t)instruction.address1.GetDataType()], instructionResultIDs[i], functionIDs[instruction.address1.GetValue()], argumentIDs);
					else
					{
						switch((IntrinsicFunction)instruction.data1)
						{
						case IntrinsicFunction::Sin:
							//OpExtInst
							irStream.AddInstruction(12, m_TypeIDs[(uint8_t)Type::Float], instructionResultIDs[i], glslStd450InstructionSetID, 13, argumentIDs);
							break;
						case IntrinsicFunction::Cos:
							//OpExtInst
							irStream.AddInstruction(12, m_TypeIDs[(uint8_t)Type::Float], instructionResultIDs[i], glslStd450InstructionSetID, 14, argumentIDs);
							break;
						case IntrinsicFunction::Tan:
							//OpExtInst
							irStream.AddInstruction(12, m_TypeIDs[(uint8_t)Type::Float], instructionResultIDs[i], glslStd450InstructionSetID, 15, argumentIDs);
							break;
						case IntrinsicFunction::Power:
							//OpExtInst
							irStream.AddInstruction(12, m_TypeIDs[(uint8_t)Type::Float], instructionResultIDs[i], glslStd450InstructionSetID, 20, argumentIDs);
							break;
						case IntrinsicFunction::NaturalExponentiation:
							//OpExtInst
							irStream.AddInstruction(12, m_TypeIDs[(uint8_t)Type::Float], instructionResultIDs[i], glslStd450InstructionSetID, 27, argumentIDs);
							break;
						case IntrinsicFunction::NaturalLogarithm:
							//OpExtInst
							irStream.AddInstruction(12, m_TypeIDs[(uint8_t)Type::Float], instructionResultIDs[i], glslStd450InstructionSetID, 28, argumentIDs);
							break;
						case IntrinsicFunction::SquareRoot:
							//OpExtInst
							irStream.AddInstruction(12, m_TypeIDs[(uint8_t)Type::Float], instructionResultIDs[i], glslStd450InstructionSetID, 31, argumentIDs);
							break;
						case IntrinsicFunction::InverseSquareRoot:
							//OpExtInst
							irStream.AddInstruction(12, m_TypeIDs[(uint8_t)Type::Float], instructionResultIDs[i], glslStd450InstructionSetID, 32, argumentIDs);
							break;
						case IntrinsicFunction::Step:
							//OpExtInst
							irStream.AddInstruction(12, m_TypeIDs[(uint8_t)Type::Float], instructionResultIDs[i], glslStd450InstructionSetID, 48, argumentIDs);
							break;
						case IntrinsicFunction::SmoothStep:
							//OpExtInst
							irStream.AddInstruction(12, m_TypeIDs[(uint8_t)Type::Float], instructionResultIDs[i], glslStd450InstructionSetID, 49, argumentIDs);
							break;
						}
					}

					break;
				}
				case Operation::FunctionCall:
					//OpFunctionCall
					irStream.AddInstruction(57, m_TypeIDs[(uint8_t)instruction.address1.GetDataType()], instructionResultIDs[i], functionIDs[instruction.address1.GetValue()]);
					break;
				}
			}

			//Magic number
			m_Instructions += 0x07230203;
			//m_Stream += 50471687;
			//Version number 1.00, but should probably be 1.3 for Vulkan API v1.1
			m_Instructions += 0x00010000;
			//Generator's magic number
			m_Instructions += 0;
			//Bound
			m_Instructions += currentID;
			//m_Stream += currentID;
			//Schema
			m_Instructions += 0;

			//Declare capabilities
			//OpCapability | Matrix
			//m_Stream.AddInstruction(17, 0);
			//OpCapability | Shader
			m_Instructions.AddInstruction(17, 1);

			//Import extended instruction sets
			//OpExtInstImport
			m_Instructions.AddInstruction(11, glslStd450InstructionSetID, PackStringLiteral("GLSL.std.450"));

			//OpMemoryModel | Addressing model: Logical | Memory model: GLSL450
			m_Instructions.AddInstruction(14, 0, 1);

			//Declare entry point
			const uint32_t executionModel = 4;
			/*
			{
				switch(i.type)
				{
				case Shader::Type::Vertex:
					executionModel = 0;
				case Shader::Type::Geometry:
					executionModel = 3;
				case Shader::Type::Fragment:
					executionModel = 4;
				}
			}
			*/

			//Result<id> of OpFunction() of program's main function | HARDCODED
			const uint32_t mainFunctionID = functionIDs[0];

			std::vector<uint32_t> entryPointArguments = PackStringLiteral("main");
			entryPointArguments.insert(entryPointArguments.end(), inputIDs.begin(), inputIDs.end());
			entryPointArguments.insert(entryPointArguments.end(), outputIDs.begin(), outputIDs.end());
			//OpEntryPoint
			m_Instructions.AddInstruction(15, executionModel, mainFunctionID, entryPointArguments);

			//Declare execution mode
			uint32_t executionMode = 7;
			/*
			{
				switch(i.type)
				{
				case Shader::Type::Vertex:
					executionMode = ;
				case Shader::Type::Geometry:
					executionMode = ;
				case Shader::Type::Fragment:
				//OriginUpperLeft | Change to 8 for OriginLowerLeft (do we want OpenGL or DirectX style UVs?)
					executionMode = 7;
				}
			}
			*/

			//optionalLiterals are things like vertex count or groupSize etc... insert them to the end of the args
			//std::vector<uint32_t> optionalLiterals = {  };
			//OpExecutionMode
			m_Instructions.AddInstruction(16, mainFunctionID, executionMode);

			for(auto &[types, identifier] : structTypeIDs)
			{
				uint32_t offset = 0;
				for(uint32_t i = 0; i < types.size(); i++)
				{
					//OpMemberDecorate | Offset
					m_DecorationInstructions.AddInstruction(72, identifier, i, 35, offset);
					offset += 16;
				}

				//OpDecorate | Block
				m_DecorationInstructions.AddInstruction(71, identifier, 2);
			}

			for(uint8_t i = 0; i < 16; i++)
			{
				if(intermediateRepresentation.descriptors[i].size())
				{
					//OpDecorate | DescriptorSet
					m_DecorationInstructions.AddInstruction(71, descriptorGroupIDs[i], 34, i);
					//OpDecorate | Binding
					m_DecorationInstructions.AddInstruction(71, descriptorGroupIDs[i], 33, 0);
					//OpVariable | Storage class: Uniform
					m_GlobalVariableDeclarationInstructions.AddInstruction(59, descriptorGroupTypeIDs[i], descriptorGroupIDs[i], 2);
				}
			}

			//OpTypePointer | Storage class: Uniform
			m_TypeDeclarationInstructions.AddInstruction(32, descriptorTypeID, 2, m_TypeIDs[(uint8_t)Type::Float]);

			for(auto &[types, typeID] : structTypeIDs)
			{
				std::vector<uint32_t> typeIDs;
				typeIDs.reserve(types.size());
				for(const Type i : types)
					typeIDs.emplace_back(m_TypeIDs[(uint8_t)i]);

				//OpTypeStruct
				m_TypeDeclarationInstructions.AddInstruction(30, typeID, typeIDs);

				//OpTypePointer | Storage class: Uniform
				m_TypeDeclarationInstructions.AddInstruction(32, typeID + 1, 2, typeID);
			}

			for(auto &[signature, typeID] : functionSignatureTypeIDs)
			{
				std::vector<uint32_t> parameterTypeIDs;
				parameterTypeIDs.reserve(signature.parameterTypes.size());
				for(const Type i : signature.parameterTypes)
					parameterTypeIDs.emplace_back(m_FunctionVariableTypeIDs[(uint8_t)i]);

				//OpTypeFunction
				m_TypeDeclarationInstructions.AddInstruction(33, typeID, m_TypeIDs[(uint8_t)signature.returnType], parameterTypeIDs);
			}

			for(uint32_t i = 0; i < structMemberAccessConstantIDs.size(); i++)
				//OpConstant
				m_ConstantInstructions.AddInstruction(43, m_TypeIDs[(uint8_t)Type::Integer], structMemberAccessConstantIDs[i], i);

			m_Instructions += m_DecorationInstructions;
			m_Instructions += m_TypeDeclarationInstructions;
			m_Instructions += m_ConstantInstructions;
			m_Instructions += m_GlobalVariableDeclarationInstructions;
			m_Instructions += irStream;
		}

		std::vector<uint32_t> CodeGenerator::PackStringLiteral(const std::string &string) const
		{
			//Pack every four UTF-8 octets into a 32-bit word

			const uint32_t characterCount = string.length() + 1;

			std::vector<uint32_t> words;
			words.resize(ceil(characterCount / 4.0f), 0);

			memcpy(words.data(), string.c_str(), characterCount);
			return words;
		}
	}
}