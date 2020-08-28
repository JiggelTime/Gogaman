#include "pch.h"
#include "VulkanCodeGenerator.h"

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

			//Reserve type IDs
			const uint32_t voidTypeID   = currentID++;
			const uint32_t floatTypeID  = currentID++;
			const uint32_t float4TypeID = currentID++;

			const uint32_t globalVariableTypeID   = currentID++;
			const uint32_t functionVariableTypeID = currentID++;

			const uint32_t inputTypeID  = currentID++;
			const uint32_t outputTypeID = currentID++;

			std::unordered_set<FunctionSignature, HashFunctionSignature>           functionSignatures;
			std::unordered_map<FunctionSignature, uint32_t, HashFunctionSignature> functionSignatureTypeIDs;
			for(auto &[functionID, signature] : intermediateRepresentation.functionSignatures)
			{
				if(!functionSignatures.contains(signature))
				{
					functionSignatureTypeIDs[signature] = currentID++;
					functionSignatures.insert(signature);
				}
			}

			const auto GetTypeID = [&](const Type type)
			{
				switch(type)
				{
				case Type::Void:
					return voidTypeID;
				case Type::Float:
					return floatTypeID;
				case Type::Float4:
					return float4TypeID;
				}
			};

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

			std::vector<uint32_t> variableIDs, functionIDs, constantIDs, instructionResultIDs;
			variableIDs.reserve(intermediateRepresentation.variableSymbolTable.size());
			functionIDs.reserve(intermediateRepresentation.functionSignatures.size());
			instructionResultIDs.resize(intermediateRepresentation.instructions.size());
			constantIDs.reserve(intermediateRepresentation.constantValues.size());
			for(uint32_t i = 0; i < intermediateRepresentation.constantValues.size(); i++)
				constantIDs.emplace_back(currentID++);

			std::vector<uint32_t> inputIDs, outputIDs;

			InstructionStream irStream;
			
			const auto GetAddressID = [&](const Address address, const bool loadVariable = false)
			{
				switch(address.GetType())
				{
				case Address::Type::Variable:
					if(loadVariable)
					{
						const uint32_t resultID = currentID++;
						//OpLoad
						irStream.AddInstruction(4, 61, GetTypeID(intermediateRepresentation.variableSymbolTable[address.GetValue()].first), resultID, { variableIDs[address.GetValue()] });
						return resultID;
					}
					else
						return variableIDs[address.GetValue()];
				case Address::Type::Function:
					return functionIDs[address.GetValue()];
				case Address::Type::Constant:
					return constantIDs[address.GetValue()];
				case Address::Type::InstructionPointer:
					return instructionResultIDs[address.GetValue()];
				}
			};

			enum class Context : uint8_t
			{
				Global,
				Function
			};

			Context context = Context::Global;
			for(uint32_t i = 0; i < intermediateRepresentation.executionOrder.size(); i++)
			{
				const uint32_t instructionIndex = intermediateRepresentation.executionOrder[i];

				if(leaderInstructionBasicBlocks.contains(instructionIndex))
					//OpLabel
					irStream.AddInstruction(2, 248, leaderInstructionBasicBlocks[instructionIndex]);

				//NOT EVERY INSTRUCTION HAS A RESULT
				instructionResultIDs[i] = currentID++;

				Instruction instruction = intermediateRepresentation.instructions[instructionIndex];
				switch(instruction.operation)
				{
				case Operation::Add:
					//OpFAdd
					irStream.AddInstruction(5, 129, GetTypeID(Type::Float), { instructionResultIDs[i], GetAddressID(instruction.address1, true), GetAddressID(instruction.address2, true) });
					break;
				case Operation::Subtract:
					//OpFSub
					irStream.AddInstruction(5, 131, GetTypeID(Type::Float), { instructionResultIDs[i], GetAddressID(instruction.address1, true), GetAddressID(instruction.address2, true) });
					break;
				case Operation::Multiply:
					//OpFMul
					irStream.AddInstruction(5, 133, GetTypeID(Type::Float), { instructionResultIDs[i], GetAddressID(instruction.address1, true), GetAddressID(instruction.address2, true) });
					break;
				case Operation::Divide:
					//OpFDiv
					irStream.AddInstruction(5, 136, GetTypeID(Type::Float), { instructionResultIDs[i], GetAddressID(instruction.address1, true), GetAddressID(instruction.address2, true) });
					break;
				case Operation::Modulo:
					//OpFMod
					irStream.AddInstruction(5, 141, GetTypeID(Type::Float), { instructionResultIDs[i], GetAddressID(instruction.address1, true), GetAddressID(instruction.address2, true) });
					break;
				case Operation::Assignment:
					//OpStore
					irStream.AddInstruction(3, 62, variableIDs[instruction.address1.GetValue()], { GetAddressID(instruction.address2, true) });
					break;
				case Operation::Branch:
					//OpBranch
					irStream.AddInstruction(2, 249, leaderInstructionBasicBlocks[instruction.address1.GetValue()]);
					break;
				case Operation::BranchOnNotEqual:
					//OpBranchConditional
					irStream.AddInstruction(4, 250, GetAddressID(instruction.address1, true), { leaderInstructionBasicBlocks[intermediateRepresentation.executionOrder[i + 1]], leaderInstructionBasicBlocks[instruction.address2.GetValue()] });
					break;
				case Operation::BeginFunction:
				{
					const FunctionSignature &signature = intermediateRepresentation.functionSignatures.at(instruction.address1.GetValue());
					//OpFunction
					irStream.AddInstruction(5, 54, GetTypeID(signature.returnType), functionIDs.emplace_back(currentID++), 0, { functionSignatureTypeIDs[signature] });
					for(auto i = 0; i < signature.parameterIDs.size(); i++)
					{
						variableIDs.emplace_back(currentID++);
						//OpFunctionParameter
						irStream.AddInstruction(3, 55, functionVariableTypeID, { variableIDs[signature.parameterIDs[i]] });
					}

					//OpLabel
					irStream.AddInstruction(2, 248, currentID++);

					uint32_t executionIndex = i + 1;
					Instruction currentInstruction = intermediateRepresentation.instructions[intermediateRepresentation.executionOrder[executionIndex]];
					while(currentInstruction.operation != Operation::EndFunction)
					{
						if((currentInstruction.operation == Operation::Assignment) && (currentInstruction.address1.GetValue() == variableIDs.size()))
						{
							const uint32_t identifier = currentID++;
							variableIDs.emplace_back(identifier);
							if(intermediateRepresentation.variableSymbolTable[currentInstruction.address1.GetValue()].second & (uint8_t)VariableSpecifierFlag::Input)
								inputIDs.emplace_back(identifier);
							else if(intermediateRepresentation.variableSymbolTable[currentInstruction.address1.GetValue()].second & (uint8_t)VariableSpecifierFlag::Output)
								outputIDs.emplace_back(identifier);
							else
								//OpVariable | Storage class: Function
								irStream.AddInstruction(4, 59, functionVariableTypeID, identifier, { 7 });
						}

						currentInstruction = intermediateRepresentation.instructions[intermediateRepresentation.executionOrder[++executionIndex]];
					}

					context = Context::Function;
					break;
				}
				case Operation::EndFunction:
					//Inject return
					//OpReturn
					//irStream.AddInstruction(1, 253);
					//OpFunctionEnd
					irStream.AddInstruction(1, 56);

					context = Context::Global;
					break;
				case Operation::Return:
					if(instruction.address1.IsValid())
						//OpReturnValue
						irStream.AddInstruction(2, 254, GetAddressID(instruction.address1, true));
					else
						//OpReturn
						irStream.AddInstruction(1, 253);
					break;
				case Operation::FunctionParameter:
				{
					std::vector<uint32_t> argumentIDs;
					while(instruction.operation == Operation::FunctionParameter)
					{
						if(instruction.address1.GetType() == Address::Type::Constant)
						{
							const uint32_t parameterID = currentID++;
							//OpVariable | Storage class: Function
							irStream.AddInstruction(5, 59, functionVariableTypeID, parameterID, 7, { constantIDs[instruction.address1.GetValue()] });
							argumentIDs.emplace_back(parameterID);
						}
						else
							argumentIDs.emplace_back(GetAddressID(instruction.address1));

						instruction = intermediateRepresentation.instructions[intermediateRepresentation.executionOrder[++i]];
						instructionResultIDs[i] = currentID++;
					}

					//OpFunctionCall
					irStream.AddInstruction(4 + argumentIDs.size(), 57, GetTypeID(intermediateRepresentation.functionSignatures.at(instruction.address1.GetValue()).returnType), instructionResultIDs[i], functionIDs[instruction.address1.GetValue()], argumentIDs);
					break;
				}
				case Operation::FunctionCall:
					//OpFunctionCall
					irStream.AddInstruction(4, 57, GetTypeID(intermediateRepresentation.functionSignatures.at(instruction.address1.GetValue()).returnType), instructionResultIDs[i], { functionIDs[instruction.address1.GetValue()] });
					break;
				}
			}

			//Magic number
			m_Stream += 0x07230203;
			//m_Stream += 50471687;
			//Version number 1.00, but should probably be 1.3 for Vulkan API v1.1
			m_Stream += 0x00010000;
			//Generator's magic number
			m_Stream += 0;
			//Bound
			//TEMP to make room for OpExtInsImport
			m_Stream += currentID + 1;
			//m_Stream += currentID;
			//Schema
			m_Stream += 0;

			//Declare capabilities
			//OpCapability | Matrix
			//m_Stream.AddInstruction(2, 17, { 0 });
			//OpCapability | Shader
			m_Stream.AddInstruction(2, 17, { 1 });

			//Import extended instruction sets
			const auto words = PackStringLiteral("GLSL.std.450");
			uint32_t std450ResultID = 1;
			//OpExtInstImport
			m_Stream.AddInstruction(2 + words.size(), 11, currentID, words);

			//OpMemoryModel | Addressing model: Logical | Memory model: GLSL450
			m_Stream.AddInstruction(3, 14, 0, { 1 });

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
			const uint32_t mainFunctionID = functionIDs[1];

			std::vector<uint32_t> entryPointArguments = PackStringLiteral("main");
			entryPointArguments.insert(entryPointArguments.end(), inputIDs.begin(), inputIDs.end());
			entryPointArguments.insert(entryPointArguments.end(), outputIDs.begin(), outputIDs.end());
			//OpEntryPoint
			m_Stream.AddInstruction(3 + entryPointArguments.size(), 15, executionModel, mainFunctionID, entryPointArguments);

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
			m_Stream.AddInstruction(3, 16, mainFunctionID, { executionMode });

			//Decorations

			for(uint32_t i = 0; i < inputIDs.size(); i++)
				//OpDecorate | Location
				m_Stream.AddInstruction(4, 71, inputIDs[i], 30, { i });

			for(uint32_t i = 0; i < outputIDs.size(); i++)
				//OpDecorate | Location
				m_Stream.AddInstruction(4, 71, outputIDs[i], 30, { i });

			//Declare types
			
			//OpTypeVoid
			m_Stream.AddInstruction(2, 19, voidTypeID);
			//OpTypeFloat | 32-bit
			m_Stream.AddInstruction(3, 22, floatTypeID, { 32 });
			//OpTypeVector | 32-bit
			m_Stream.AddInstruction(4, 23, float4TypeID, floatTypeID, { 4 });

			//OpTypePointer | Storage class: Private
			m_Stream.AddInstruction(4, 32, globalVariableTypeID, 6, { GetTypeID(Type::Float) });
			//OpTypePointer | Storage class: Function
			m_Stream.AddInstruction(4, 32, functionVariableTypeID, 7, { GetTypeID(Type::Float) });

			if(inputIDs.size())
				//OpTypePointer | Storage class: Input
				m_Stream.AddInstruction(4, 32, inputTypeID, 1, { GetTypeID(Type::Float) });

			if(outputIDs.size())
				//OpTypePointer | Storage class: Output
				m_Stream.AddInstruction(4, 32, outputTypeID, 3, { GetTypeID(Type::Float) });

			for(auto &[signature, typeID] : functionSignatureTypeIDs)
			{
				std::vector<uint32_t> parameterTypeIDs;
				parameterTypeIDs.reserve(signature.parameterTypes.size());
				for(const Type i : signature.parameterTypes)
					parameterTypeIDs.emplace_back(functionVariableTypeID);

				//OpTypeFunction
				m_Stream.AddInstruction(3 + signature.parameterTypes.size(), 33, typeID, GetTypeID(signature.returnType), parameterTypeIDs);
			}

			//Constants
			for(uint32_t i = 0; i < constantIDs.size(); i++)
				//OpConstant
				m_Stream.AddInstruction(3 + 1, 43, GetTypeID(Type::Float), constantIDs[i], { intermediateRepresentation.constantValues[i] });

			//Global variables

			for(const uint32_t i : inputIDs)
				//OpVariable | Storage class: Input
				m_Stream.AddInstruction(4, 59, inputTypeID, i, { 1 });

			for(const uint32_t i : outputIDs)
				//OpVariable | Storage class: Output
				m_Stream.AddInstruction(4, 59, outputTypeID, i, { 3 });

			m_Stream += irStream;
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