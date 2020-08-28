#pragma once

#include "Visitor.h"

namespace Gogaman
{
	namespace FlexShader
	{
		enum class Type : uint8_t;

		namespace AST
		{
			class SemanticAnalyzer
			{
			public:
				SemanticAnalyzer()
					: m_CurrentScopeDepth(0)
				{}

				~SemanticAnalyzer() = default;

				Type VisitAbstract(Node::Abstract &node);
				Type VisitExpression(Node::Expression &node);
				Type VisitStatement(Node::Statement &node);
				Type VisitStatementBlock(Node::StatementBlock &node);
				Type VisitFunctionPrototype(Node::FunctionPrototype &node);
				Type VisitFunction(Node::Function &node);
				Type VisitFunctionCall(Node::FunctionCall &node);
				Type VisitComponent(Node::Component &node);
				Type VisitVariableDeclaration(Node::VariableDeclaration &node);
				Type VisitComponentInstantiation(Node::ComponentInstantiation &node);
				Type VisitNumericLiteral(Node::NumericLiteral &node);
				Type VisitStringLiteral(Node::StringLiteral &node);
				Type VisitIdentifier(Node::Identifier &node);
				Type VisitBinaryOperation(Node::BinaryOperation &node);
				Type VisitAssignment(Node::Assignment &node);
				Type VisitBranch(Node::Branch &node);
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