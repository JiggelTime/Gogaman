#include "pch.h"
#include "Generator.h"

#include "Gogaman/Rendering/FlexShader/AbstractSyntaxTree.h"

namespace Gogaman
{
	namespace FlexShader
	{
		namespace AST
		{
			SPIRV_Visitor::SPIRV_Visitor(AST::Node::Abstract *rootNode, const std::string &entryPointName)
				: m_CurrentID(0), m_IsGlobalNamespace(true)
			{
				rootNode->Accept(*this);

				//Magic number
				m_Stream += 0x07230203;

				//Version number
				//1.00 but should probably be 1.3 for Vulkan API v1.1
				m_Stream += 0x00010000;

				//Generator's magic number
				//Gogaman FlexShader generator is just 0
				m_Stream += 0;

				//Bound
				auto constants = m_FloatConstantIDs.size();
				auto fncTypes = m_FunctionTypeIDs.size();
				auto ptrTypes = m_PointerTypeIDs.size();
				m_CurrentID;
				m_Stream += m_CurrentID;

				//Reserved word
				m_Stream += 0;

				//Instruction stream start

				//Declare capabilities
				//Matrix
				m_Stream.AddInstruction(2, 17, { 0 });
				//Shader
				m_Stream.AddInstruction(2, 17, { 1 });

				//Import extended instruction sets
				const auto words = PackStringLiteral("GLSL.std.450");
				uint32_t std450ResultID = 1;
				m_Stream.AddInstruction(3 + words.size(), 11, std450ResultID, words);

				//Set addressing/memory model
				//Logcal addressing model and GLSL450 memory model
				m_Stream.AddInstruction(3, 14, { 0, 1 });

				//Declare entry point
				//for(auto i : root.programs)
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

				//Result<id> of OpFunction() of program's main function
				const uint32_t mainFunctionID = 2;

				std::vector<uint32_t> args = { executionModel, mainFunctionID };
				const auto mainLiteralPacked = PackStringLiteral("main");
				args.insert(args.end(), mainLiteralPacked.begin(), mainLiteralPacked.end());
				//also insert <id> of global OpVariable for inputs/outputs of the entry point (eg. v_Color input, p_Color output)
				//The wordCount might not be right for this. It says "4 + variable" in the spec
				m_Stream.AddInstruction(4 + mainLiteralPacked.size(), 15, args);

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
				m_Stream.AddInstruction(3, 16, { mainFunctionID, executionMode });

				//Decorations
				//for(const auto i : program.descriptors){
				//const auto decorationNamePacked = PackStringLiteral(i.name);
				//OpDecorate
				//EmitInstruction(3 + decorationNamePacked.size(), 71, 
				//}

				//Declare types
				uint32_t voidTypeID = 0, floatTypeID = 0, float4TypeID = 0;
				//Void
				m_Stream.AddInstruction(2, 19, voidTypeID);
				//Float (32-bit)
				m_Stream.AddInstruction(2, 22, floatTypeID, { 32 });
				//Float4 (32-bit)
				m_Stream.AddInstruction(4, 23, float4TypeID, { floatTypeID, 4 });

				//Constants
				//Eg. float with value 0.35
				uint32_t testFloatConstID = 2;
				m_GlobalStream.AddInstruction(3 + 1, 43, { floatTypeID, testFloatConstID, (uint32_t)0.35f });

				//Global variables

				m_Stream += m_GlobalStream;

				//Functions
				for(const auto &i : m_FunctionStreams)
					m_Stream += i;
			}

			std::vector<uint32_t> SPIRV_Visitor::PackStringLiteral(const std::string &string) const
			{
				//Packs every four UTF-8 octets into a 32-bit word

				//Needs to be uint32_t if there can be > 65.5k chars
				const uint16_t characterCount = string.length() + 1;

				std::vector<uint32_t> words;
				words.resize(ceil(characterCount / 4.0f), 0);

				memcpy(words.data(), string.c_str(), characterCount);
				return words;
			}

			uint32_t SPIRV_Visitor::VisitStatementBlock(Node::StatementBlock &node)
			{
				for(Node::Statement *i : node.statements)
					i->Accept(*this);

				return UINT32_MAX;
			}

			uint32_t SPIRV_Visitor::VisitFunctionPrototype(Node::FunctionPrototype &node)
			{
				if(!m_FunctionTypeIDs.contains(&node))
				{
					std::vector<uint32_t> argumentTypeIDs;
					argumentTypeIDs.reserve(node.arguments.size());
					for(const Node::VariableDeclaration *i : node.arguments)
						argumentTypeIDs.emplace_back(m_SymbolTypeIDs[i->type]);

					//TODO: Use hash table to avoid duplicate OpTypeFunction for functions with same return and argument types
					//OpTypeFunction
					m_GlobalStream.AddInstruction(3 + argumentTypeIDs.size(), 33, m_CurrentID, argumentTypeIDs);
					m_FunctionTypeIDs[&node] = m_CurrentID++;
				}

				//OpFunction
				m_FunctionStreams.back().AddInstruction(5, 54, m_SymbolTypeIDs[node.returnType], { m_CurrentID, 0, m_FunctionTypeIDs[&node] });
				const uint32_t functionID = m_CurrentID++;
				m_SymbolIDs[node.name] = functionID;

				for(const Node::VariableDeclaration *i : node.arguments)
				{
					//OpFunctionParameter
					m_FunctionStreams.back().AddInstruction(3, 55, m_SymbolTypeIDs[i->type], { m_CurrentID });
					//Argument name prefixed with function name to avoid collisions
					m_SymbolIDs[node.name + '$' + i->name] = m_CurrentID++;
				}

				m_FunctionReturnTypes[node.name] = node.returnType;

				return functionID;
			}

			uint32_t SPIRV_Visitor::VisitFunction(Node::Function &node)
			{
				InstructionStream &functionStream = m_FunctionStreams.emplace_back();

				const uint32_t functionID = node.prototype->Accept(*this);

				m_CurrentFunctionBlocks.emplace_back();
				m_IsGlobalNamespace = false;
				node.body->Accept(*this);
				m_IsGlobalNamespace = true;

				//First block
				//OpLabel
				functionStream.AddInstruction(2, 248, m_CurrentID);
				const uint32_t blockID = m_CurrentID++;

				for(const FunctionVariable &i : m_CurrentFunctionVariables)
				{
					if(!m_PointerTypeIDs.contains({ 7, i.declaration->type }))
					{
						//OpTypePointer | Storage class: Function
						m_GlobalStream.AddInstruction(4, 32, m_CurrentID, { 7, m_SymbolTypeIDs[i.declaration->type] });
						m_PointerTypeIDs[{ 7, i.declaration->type }] = m_CurrentID++;
					}

					//OpVariable | Storage class: Function
					if(i.valueID != UINT32_MAX)
						functionStream.AddInstruction(5, 59, m_PointerTypeIDs[{ 7, i.declaration->type }], { i.identifier, 7, i.valueID });
					else
						functionStream.AddInstruction(4, 59, m_PointerTypeIDs[{ 7, i.declaration->type }], { i.identifier, 7 });

					//Variable name prefixed with function name to avoid collisions
					m_SymbolIDs[node.prototype->name + '$' + i.declaration->name] = i.identifier;
				}

				m_CurrentFunctionVariables.clear();

				//Append block instructions to function stream
				functionStream += m_CurrentFunctionBlocks.front();

				if(m_CurrentFunctionBlocks.size() > 1)
				{
					for(auto i = m_CurrentFunctionBlocks.begin() + 1; i != m_CurrentFunctionBlocks.end(); i++)
					{
						//OpLabel
						functionStream.AddInstruction(2, 248, m_CurrentID);
						const uint32_t blockID = m_CurrentID++;

						//Append block instructions to function stream
						functionStream += *i;
					}
				}

				m_CurrentFunctionBlocks.clear();

				//OpFunctionEnd
				functionStream.AddInstruction(1, 56);

				return functionID;
			}

			uint32_t SPIRV_Visitor::VisitFunctionCall(Node::FunctionCall &node)
			{
				GM_DEBUG_ASSERT(!m_IsGlobalNamespace, "Invalid FlexShader function call | Function call is in global namespace");

				std::vector<uint32_t> operands;
				operands.reserve(1 + node.arguments.size());
				const uint32_t resultID = m_CurrentID++;
				operands.emplace_back(resultID);
				for(Node::Expression *i : node.arguments)
					operands.emplace_back(i->Accept(*this));

				//OpFunctionCall
				m_CurrentFunctionBlocks.back().AddInstruction(4 + node.arguments.size(), 57, m_SymbolTypeIDs[m_FunctionReturnTypes[node.name]], operands);

				return resultID;
			}

			uint32_t SPIRV_Visitor::VisitVariableDeclaration(Node::VariableDeclaration &node)
			{
				if(m_IsGlobalNamespace)
				{
					return UINT32_MAX;
				}
				else
				{
					const uint32_t identifier = m_CurrentID++;
					m_CurrentFunctionVariables.emplace_back(FunctionVariable{ &node, identifier });
					return identifier;
				}
			}

			uint32_t SPIRV_Visitor::VisitNumericLiteral(Node::NumericLiteral &node)
			{
				const uint32_t identifier = m_CurrentID++;
				m_FloatConstantIDs[stof(node.value)] = identifier;
				return identifier;
			}

			uint32_t SPIRV_Visitor::VisitStringLiteral(Node::StringLiteral &node)
			{
				return UINT32_MAX;
			}

			uint32_t SPIRV_Visitor::VisitIdentifier(Node::Identifier &node)
			{
				GM_DEBUG_ASSERT(m_SymbolIDs.contains(node.name), "Invalid FlexShader identifier");
				return m_SymbolIDs[node.name];
			}

			uint32_t SPIRV_Visitor::VisitBinaryOperation(Node::BinaryOperation &node)
			{
				GM_DEBUG_ASSERT(!m_IsGlobalNamespace, "Invalid FlexShader binary operation | Binary operation is in global namespace");

				const uint32_t resultID = m_CurrentID++;

				//TODO: Need to know the type of the operands (float, vector, integer). For now assume everything is float
				switch(node.operatorType)
				{
				case Token::Type::Plus:
					//OpFAdd
					m_CurrentFunctionBlocks.back().AddInstruction(5, 129, m_SymbolTypeIDs[Type::Float], { resultID, node.leftOperand->Accept(*this), node.rightOperand->Accept(*this) });
					break;
				case Token::Type::Minus:
					//OpFSub
					m_CurrentFunctionBlocks.back().AddInstruction(5, 131, m_SymbolTypeIDs[Type::Float], { resultID, node.leftOperand->Accept(*this), node.rightOperand->Accept(*this) });
					break;
				case Token::Type::Asterisk:
					//OpFMul
					m_CurrentFunctionBlocks.back().AddInstruction(5, 133, m_SymbolTypeIDs[Type::Float], { resultID, node.leftOperand->Accept(*this), node.rightOperand->Accept(*this) });
					break;
				case Token::Type::Slash:
					//OpFDiv
					m_CurrentFunctionBlocks.back().AddInstruction(5, 136, m_SymbolTypeIDs[Type::Float], { resultID, node.leftOperand->Accept(*this), node.rightOperand->Accept(*this) });
					break;
				case Token::Type::Percent:
					//OpFMod
					m_CurrentFunctionBlocks.back().AddInstruction(5, 141, m_SymbolTypeIDs[Type::Float], { resultID, node.leftOperand->Accept(*this), node.rightOperand->Accept(*this) });
					break;
				}

				return resultID;
			}

			uint32_t SPIRV_Visitor::VisitAssignment(Node::Assignment &node)
			{
				GM_DEBUG_ASSERT(!m_IsGlobalNamespace, "Invalid FlexShader assignment | Assignment is in global namespace");

				const uint32_t lValueID = node.lValue->Accept(*this);
				if(node.operatorType == Token::Type::Equal)
					//OpStore
					m_CurrentFunctionBlocks.back().AddInstruction(3, 62, lValueID, { node.rValue->Accept(*this) });

				return lValueID;
			}
		}
	}
}