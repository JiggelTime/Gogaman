#pragma once

#include "IntermediateRepresentation.h"

#include "Visitor.h"

namespace Gogaman
{
	namespace FlexShader
	{
		class Translator
		{
		public:
			Translator()
				: m_VariableCount(0), m_FunctionCount(0), m_ConstantCount(0)
			{ m_SymbolTables.emplace_back(); }

			~Translator() = default;

			inline IR::Address VisitAbstract(AST::Node::Abstract &node) { return {}; }
			inline IR::Address VisitExpression(AST::Node::Expression &node) { return {}; }
			inline IR::Address VisitStatement(AST::Node::Statement &node) { return {}; }
			IR::Address VisitStatementBlock(AST::Node::StatementBlock &node);
			IR::Address VisitFunctionPrototype(AST::Node::FunctionPrototype &node);
			IR::Address VisitFunction(AST::Node::Function &node);
			IR::Address VisitFunctionCall(AST::Node::FunctionCall &node);
			inline IR::Address VisitComponent(AST::Node::Component &node) { return {}; }
			IR::Address VisitVariableDeclaration(AST::Node::VariableDeclaration &node);
			inline IR::Address VisitComponentInstantiation(AST::Node::ComponentInstantiation &node) { return {}; }
			IR::Address VisitNumericLiteral(AST::Node::NumericLiteral &node);
			IR::Address VisitStringLiteral(AST::Node::StringLiteral &node);
			IR::Address VisitIdentifier(AST::Node::Identifier &node);
			IR::Address VisitBinaryOperation(AST::Node::BinaryOperation &node);
			IR::Address VisitAssignment(AST::Node::Assignment &node);
			IR::Address VisitBranch(AST::Node::Branch &node);
			IR::Address VisitReturn(AST::Node::Return &node);

			inline void RegisterSymbolAddress(const std::string name, const IR::Address address) { m_SymbolTables.back()[name] = address; }
			IR::Address GetSymbolAddress(const std::string name) const;

			inline void AddInstruction(const IR::Operation operation, const IR::Address address1 = {}, const IR::Address address2 = {}) { m_IR.instructions.emplace_back(IR::Instruction{ address1, address2, operation }); }

			const IR::IR &IR();
		private:
			IR::IR m_IR;

			std::vector<std::unordered_map<std::string, IR::Address>> m_SymbolTables;
			
			uint32_t m_VariableCount, m_FunctionCount, m_ConstantCount;
		};
	}
}