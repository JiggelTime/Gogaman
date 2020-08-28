#pragma once

#include "Gogaman/Rendering/FlexShader/Visitor.h"

namespace Gogaman
{
	namespace FlexShader
	{
		namespace AST
		{
			class SPIRV_Visitor
			{
			private:
				struct HashPointerType
				{
					inline size_t operator()(const std::pair<uint8_t, Type> &pointerType) const { return std::hash<uint16_t>()((uint16_t)pointerType.first | ((uint16_t)pointerType.second << 8)); }
				};

				struct FunctionVariable
				{
					Node::VariableDeclaration *declaration;
					uint32_t                   identifier, valueID = UINT32_MAX;
				};

				class InstructionStream
				{
				public:
					InstructionStream()  = default;
					~InstructionStream() = default;

					InstructionStream &operator+=(const InstructionStream &other)
					{
						m_Stream.insert(m_Stream.end(), other.m_Stream.begin(), other.m_Stream.end());
						return *this;
					}

					InstructionStream &operator+=(const uint32_t value)
					{
						m_Stream.emplace_back(value);
						return *this;
					}

					inline void AddInstruction(const uint16_t wordCount, const uint16_t opCode, const std::vector<uint32_t> &operands = {})
					{
						m_Stream.emplace_back(((uint32_t)wordCount << 16) | (uint32_t)opCode);
						for(const uint32_t i : operands)
							m_Stream.emplace_back(i);
					}

					inline void AddInstruction(const uint16_t wordCount, const uint16_t opCode, const uint32_t result, const std::vector<uint32_t> &operands = {})
					{
						m_Stream.emplace_back(((uint32_t)wordCount << 16) | (uint32_t)opCode);
						m_Stream.emplace_back(result);
						for(const uint32_t i : operands)
							m_Stream.emplace_back(i);
					}

					inline const uint32_t *GetStream() const { return m_Stream.data(); }
				private:
					std::vector<uint32_t> m_Stream;
				};
			public:
				SPIRV_Visitor(Node::Abstract *rootNode, const std::string &entryPointName);
				~SPIRV_Visitor() = default;

				inline uint32_t VisitAbstract(Node::Abstract &node) { return UINT32_MAX; }
				inline uint32_t VisitExpression(Node::Expression &node) { return UINT32_MAX; }
				inline uint32_t VisitStatement(Node::Statement &node) { return UINT32_MAX; }
				uint32_t VisitStatementBlock(Node::StatementBlock &node);
				uint32_t VisitFunctionPrototype(Node::FunctionPrototype &node);
				uint32_t VisitFunction(Node::Function &node);
				uint32_t VisitFunctionCall(Node::FunctionCall &node);
				inline uint32_t VisitComponent(Node::Component &node) { return UINT32_MAX; }
				uint32_t VisitVariableDeclaration(Node::VariableDeclaration &node);
				inline uint32_t VisitComponentInstantiation(Node::ComponentInstantiation &node) { return UINT32_MAX; }
				uint32_t VisitNumericLiteral(Node::NumericLiteral &node);
				uint32_t VisitStringLiteral(Node::StringLiteral &node);
				uint32_t VisitIdentifier(Node::Identifier &node);
				uint32_t VisitBinaryOperation(Node::BinaryOperation &node);
				uint32_t VisitAssignment(Node::Assignment &node);

				inline uint8_t *GetBytecode() const { return (uint8_t *)m_Stream.GetStream(); }
			private:
				std::vector<uint32_t> PackStringLiteral(const std::string &string) const;
			private:
				InstructionStream              m_Stream;
				InstructionStream              m_GlobalStream;
				std::vector<InstructionStream> m_FunctionStreams;

				std::unordered_map<float, uint32_t> m_FloatConstantIDs;

				std::unordered_map<Type, uint32_t>                                       m_SymbolTypeIDs;
				std::unordered_map<Node::FunctionPrototype *, uint32_t>                  m_FunctionTypeIDs;
				std::unordered_map<std::pair<uint32_t, Type>, uint32_t, HashPointerType> m_PointerTypeIDs;

				std::unordered_map<std::string, uint32_t> m_SymbolIDs;

				std::vector<FunctionVariable>  m_CurrentFunctionVariables;
				std::vector<InstructionStream> m_CurrentFunctionBlocks;

				std::unordered_map<std::string, Type> m_FunctionReturnTypes;

				uint32_t m_CurrentID;

				bool m_IsGlobalNamespace;
			};
		}
	}
}