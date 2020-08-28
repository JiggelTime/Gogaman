#include "pch.h"
#include "SemanticAnalyzer.h"

#include "Gogaman/Core/Logging/Log.h"

#include "Type.h"
#include "AbstractSyntaxTree.h"

namespace Gogaman
{
	namespace FlexShader
	{
		namespace AST
		{
			Type SemanticAnalyzer::VisitAbstract(Node::Abstract &node)     { return Type::None; }
			Type SemanticAnalyzer::VisitExpression(Node::Expression &node) { return Type::None; }
			Type SemanticAnalyzer::VisitStatement(Node::Statement &node)   { return Type::None; }

			Type SemanticAnalyzer::VisitStatementBlock(Node::StatementBlock &node)
			{
				for(const auto i : node.statements)
					i->Accept(*this);

				return Type::None;
			}

			Type SemanticAnalyzer::VisitFunctionPrototype(Node::FunctionPrototype &node)
			{
				//GM_DEBUG_ASSERT(!m_SymbolTable.contains(node.name), "Failed to validate FlexShader function prototype | Symbol \"%s\" redefinition", node.name.c_str());

				m_SymbolTables[m_CurrentScopeDepth - 1][node.name] = node.returnType;

				for(const auto i : node.parameters)
					i->Accept(*this);

				return node.returnType;
			}

			Type SemanticAnalyzer::VisitFunction(Node::Function &node)
			{
				//Assert that current depth is < 2
				m_CurrentScopeDepth++;

				const Type returnType = node.prototype->Accept(*this);

				if(node.body)
					node.body->Accept(*this);

				m_SymbolTables[m_CurrentScopeDepth].clear();
				m_CurrentScopeDepth--;

				return returnType;
			}

			Type SemanticAnalyzer::VisitFunctionCall(Node::FunctionCall &node) { return GetType(node.name); }

			Type SemanticAnalyzer::VisitComponent(Node::Component &node)
			{
				//Assert that current depth is 0
				m_CurrentScopeDepth++;

				node.body->Accept(*this);

				m_SymbolTables[m_CurrentScopeDepth].clear();
				m_CurrentScopeDepth--;

				return Type::None;
			}

			Type SemanticAnalyzer::VisitVariableDeclaration(Node::VariableDeclaration &node)
			{
				//GM_DEBUG_ASSERT(!m_SymbolTable.contains(node.name), "Failed to validate FlexShader variable declaration | Symbol \"%s\" redefinition", node.name.c_str());

				m_SymbolTables[m_CurrentScopeDepth][node.name] = node.type;

				return node.type;
			}

			Type SemanticAnalyzer::VisitComponentInstantiation(Node::ComponentInstantiation &node) { return Type::None; }

			Type SemanticAnalyzer::VisitNumericLiteral(Node::NumericLiteral &node)
			{
				//Assume that all numeric literals are floating point literals
				//Eventually need to add seperate token type for integer literals
				return Type::Float;
			}

			Type SemanticAnalyzer::VisitStringLiteral(Node::StringLiteral &node) { return Type::None; }

			Type SemanticAnalyzer::VisitIdentifier(Node::Identifier &node) { return GetType(node.name); }

			Type SemanticAnalyzer::VisitBinaryOperation(Node::BinaryOperation &node)
			{
				const Type leftType = node.leftOperand->Accept(*this);
				const Type rightType = node.rightOperand->Accept(*this);

				GM_DEBUG_ASSERT(leftType == rightType, "Failed to validate FlexShader binary operation | Left operand type %s does not match right operand type %s", GetTypeString(leftType).c_str(), GetTypeString(rightType).c_str());

				return leftType;
			}

			Type SemanticAnalyzer::VisitAssignment(Node::Assignment &node)
			{
				const Type leftType = node.lValue->Accept(*this);
				const Type rightType = node.rValue->Accept(*this);

				GM_DEBUG_ASSERT(leftType == rightType, "Failed to validate FlexShader assignment | Left type \"%s\" incompatible right type \"%s\"", GetTypeString(leftType).c_str(), GetTypeString(rightType).c_str());

				return leftType;
			}

			Type SemanticAnalyzer::VisitBranch(Node::Branch &node) { return Type::None; }

			Type SemanticAnalyzer::VisitReturn(Node::Return &node)
			{
				if(node.value)
					return node.value->Accept(*this);
				else
					return Type::Void;
			}

			Type SemanticAnalyzer::GetType(const std::string &symbol)
			{
				uint8_t i = m_CurrentScopeDepth;
				while(true)
				{
					if(m_SymbolTables[i].contains(symbol))
						return m_SymbolTables[i][symbol];

					if(i-- == 0)
						break;
				}

				GM_DEBUG_ASSERT(false, "Failed to get FlexShader symbol type | Undefined symbol \"%s\"", symbol.c_str());
			}
		}
	}
}