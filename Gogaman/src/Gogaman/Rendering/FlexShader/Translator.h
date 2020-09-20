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
				: m_VariableCount(0), m_FunctionCount(0)
			{
				m_SymbolTables.emplace_back();
			}

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
			IR::Address VisitBooleanLiteral(AST::Node::BooleanLiteral &node);
			IR::Address VisitIntegerLiteral(AST::Node::IntegerLiteral &node);
			IR::Address VisitFloatingPointLiteral(AST::Node::FloatingPointLiteral &node);
			inline IR::Address VisitStringLiteral(AST::Node::StringLiteral &node) { return {}; }
			IR::Address VisitIdentifier(AST::Node::Identifier &node);
			IR::Address VisitVector(AST::Node::Vector &node);
			IR::Address VisitMemberSelection(AST::Node::MemberSelection &node);
			IR::Address VisitBinaryOperation(AST::Node::BinaryOperation &node);
			IR::Address VisitAssignment(AST::Node::Assignment &node);
			IR::Address VisitBranch(AST::Node::Branch &node);
			IR::Address VisitReturn(AST::Node::Return &node);

			inline void RegisterSymbolAddress(const std::string name, const IR::Address address) { m_SymbolTables.back()[name] = address; }
			IR::Address GetSymbolAddress(const std::string name) const;

			template<typename ...ParameterTypes>
			inline void AddInstruction(ParameterTypes &&...constructorParameters) { m_IR.instructions.emplace_back(std::forward<ParameterTypes>(constructorParameters)...); }

			const IR::IR &IR();
		private:
			IR::IR m_IR;

			std::vector<std::unordered_map<std::string, IR::Address>> m_SymbolTables;
			
			uint32_t m_VariableCount, m_FunctionCount;
		};
	}
}