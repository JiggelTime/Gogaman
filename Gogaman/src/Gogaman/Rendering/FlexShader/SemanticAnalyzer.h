#pragma once

#include "Type.h"
#include "Visitor.h"

namespace Gogaman
{
	namespace FlexShader
	{
		namespace AST
		{
			class SemanticAnalyzer
			{
			public:
				SemanticAnalyzer()
					: m_CurrentScopeDepth(0)
				{}

				~SemanticAnalyzer() = default;

				inline constexpr Type VisitAbstract(Node::Abstract &node)     { return Type::Void; }
				inline constexpr Type VisitExpression(Node::Expression &node) { return Type::Void; }
				inline constexpr Type VisitStatement(Node::Statement &node)   { return Type::Void; }
				Type VisitStatementBlock(Node::StatementBlock &node);
				Type VisitFunctionPrototype(Node::FunctionPrototype &node);
				Type VisitFunction(Node::Function &node);
				Type VisitFunctionCall(Node::FunctionCall &node);
				Type VisitComponent(Node::Component &node);
				Type VisitVariableDeclaration(Node::VariableDeclaration &node);
				inline constexpr Type VisitComponentInstantiation(Node::ComponentInstantiation &node) { return Type::Void; }
				inline constexpr Type VisitBooleanLiteral(Node::BooleanLiteral &node)             { return Type::Boolean; }
				inline constexpr Type VisitIntegerLiteral(Node::IntegerLiteral &node)             { return Type::Integer; }
				inline constexpr Type VisitFloatingPointLiteral(Node::FloatingPointLiteral &node) { return Type::Float; }
				inline constexpr Type VisitStringLiteral(Node::StringLiteral &node)               { return Type::Void; }
				Type VisitIdentifier(Node::Identifier &node);
				Type VisitVector(Node::Vector &node);
				Type VisitMemberSelection(Node::MemberSelection &node);
				Type VisitBinaryOperation(Node::BinaryOperation &node);
				Type VisitAssignment(Node::Assignment &node);
				inline constexpr Type VisitBranch(Node::Branch &node) { return Type::Void; }
				Type VisitReturn(Node::Return &node);
			private:
				Type GetType(const std::string &symbol);
			private:
				//Global, component, function namespaces
				std::unordered_map<std::string, Type> m_SymbolTables[3];
				uint8_t                               m_CurrentScopeDepth;
			};
		}
	}
}