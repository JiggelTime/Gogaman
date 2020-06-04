#pragma once

#include "Gogaman/Rendering/FlexShader/Visitor.h"
#include "Gogaman/Rendering/FlexShader/AbstractSyntaxTree.h"

namespace Gogaman
{
	namespace FlexShader
	{
		namespace AST
		{
			class SPIRV_Visitor : public AbstractVisitor<SPIRV_Visitor>
			{
			public:
				SPIRV_Visitor()  = default;
				~SPIRV_Visitor() = default;

				void VisitStatementBlock(Node::StatementBlock &node);
				void VisitVariableDeclaration(Node::VariableDeclaration &node);
				void VisitFunctionPrototype(Node::FunctionPrototype &node);
				void VisitFunction(Node::Function &node);
				void VisitNumericLiteral(Node::NumericLiteral &node);
				void VisitStringLiteral(Node::StringLiteral &node);
				void VisitIdentifier(Node::Identifier &node);
				void VisitBinaryOperation(Node::BinaryOperation &node);
				void VisitFunctionCall(Node::FunctionCall &node);
				void VisitAssignment(Node::Assignment &node);
			private:
				
			};
		}

		class SPIRV_Generator
		{
		public:
			SPIRV_Generator(const AST::Node::Abstract *rootNode);
			~SPIRV_Generator() = default;
		private:
			std::vector<uint32_t> PackStringLiteral(const std::string &string) const;

			void EmitInstruction(const uint16_t wordCount, const uint16_t opCode, const std::vector<uint32_t> &operands = {});
			void EmitInstruction(const uint16_t wordCount, const uint16_t opCode, const uint32_t result, const std::vector<uint32_t> &operands = {});
		private:
			std::vector<uint32_t> m_Module;
		};
	}
}