#include "pch.h"
#include "Visitor.h"

#include "Gogaman/Core/Logging/Log.h"

#include "AbstractSyntaxTree.h"

namespace Gogaman
{
	namespace FlexShader
	{
		namespace AST
		{
			void PrintVisitor::VisitStatementBlock(Node::StatementBlock &node)
			{
				std::string spaces;
				spaces.append(m_Depth++ * 2, ' ');

				GM_LOG_CORE_INFO("%sStatement block | Statements:", spaces.c_str());

				for(auto i : node.statements)
				{
					i->Accept(*this);
				}

				m_Depth--;
			}

			void PrintVisitor::VisitFunctionPrototype(Node::FunctionPrototype &node)
			{
				std::string spaces;
				spaces.append(m_Depth * 2, ' ');

				std::string argumentNames;
				for(auto i : node.argumentNames)
				{
					argumentNames.append(i).append(", ");
				}

				GM_LOG_CORE_INFO("%sFunction Prototype | Name: %s | Return type: %s | Argument names: %s", spaces.c_str(), node.name.c_str(), GetTypeString(node.returnType).c_str(), argumentNames.c_str());
			}

			void PrintVisitor::VisitFunction(Node::Function &node)
			{
				std::string spaces;
				spaces.append(m_Depth++ * 2, ' ');

				GM_LOG_CORE_INFO("%sFunction:", spaces.c_str());
				node.prototype->Accept(*this);

				GM_LOG_CORE_INFO("%s  Body:", spaces.c_str());
				m_Depth++;
				node.body->Accept(*this);

				m_Depth -= 2;
			}

			void PrintVisitor::VisitFunctionCall(Node::FunctionCall &node)
			{
				std::string spaces;
				spaces.append(m_Depth++ * 2, ' ');

				GM_LOG_CORE_INFO("%sFunction call | Name: %s | Arguments:", spaces.c_str(), node.name.c_str());
				for(auto i : node.arguments)
				{
					i->Accept(*this);
				}

				m_Depth--;
			}

			void PrintVisitor::VisitComponent(Node::Component &node)
			{
				std::string spaces;
				spaces.append(m_Depth++ * 2, ' ');

				GM_LOG_CORE_INFO("%sComponent | Name: %s", spaces.c_str(), node.name.c_str());

				GM_LOG_CORE_INFO("%s  Body:", spaces.c_str());
				m_Depth++;
				node.body->Accept(*this);

				m_Depth -= 2;
			}

			void PrintVisitor::VisitVariableDeclaration(Node::VariableDeclaration &node)
			{
				std::string spaces;
				spaces.append(m_Depth * 2, ' ');

				GM_LOG_CORE_INFO("%sVariable declaration | Name: %s | Type: %s", spaces.c_str(), node.name.c_str(), GetTypeString(node.type).c_str());
			}

			void PrintVisitor::VisitNumericLiteral(Node::NumericLiteral &node)
			{
				std::string spaces;
				spaces.append(m_Depth * 2, ' ');

				GM_LOG_CORE_INFO("%sNumeric literal | Value: %s", spaces.c_str(), std::string(node.value).c_str());
			}

			void PrintVisitor::VisitStringLiteral(Node::StringLiteral &node)
			{
				std::string spaces;
				spaces.append(m_Depth * 2, ' ');

				GM_LOG_CORE_INFO("%sString literal | Value: %s", spaces.c_str(), std::string(node.value).c_str());
			}

			void PrintVisitor::VisitIdentifier(Node::Identifier &node)
			{
				std::string spaces;
				spaces.append(m_Depth * 2, ' ');

				GM_LOG_CORE_INFO("%sIdentifier | Name: %s", spaces.c_str(), std::string(node.name).c_str());
			}

			void PrintVisitor::VisitBinaryOperation(Node::BinaryOperation &node)
			{
				std::string spaces;
				spaces.append(m_Depth++ * 2, ' ');

				GM_LOG_CORE_INFO("%sBinary operation | Type: %s", spaces.c_str(), GetTokenTypeString(node.operatorType).c_str());
				node.leftOperand->Accept(*this);
				node.rightOperand->Accept(*this);

				m_Depth--;
			}

			void PrintVisitor::VisitAssignment(Node::Assignment &node)
			{
				std::string spaces;
				spaces.append(m_Depth++ * 2, ' ');

				GM_LOG_CORE_INFO("%sAssignment | Operator: %s | Left operand:", spaces.c_str(), GetTokenTypeString(node.operatorType).c_str());
				node.lValue->Accept(*this);

				GM_LOG_CORE_INFO("%s  Right operand:", spaces.c_str());
				m_Depth += 2;
				node.rValue->Accept(*this);

				m_Depth -= 3;
			}

			Type ValidationVisitor::VisitAbstract(Node::Abstract &node) { return Type::None; }

			Type ValidationVisitor::VisitExpression(Node::Expression &node) { return Type::None; }

			Type ValidationVisitor::VisitStatement(Node::Statement &node) { return Type::None; }

			Type ValidationVisitor::VisitStatementBlock(Node::StatementBlock &node)
			{
				for(auto i : node.statements)
				{
					i->Accept(*this);
				}

				return Type::None;
			}

			Type ValidationVisitor::VisitFunctionPrototype(Node::FunctionPrototype &node)
			{
				GM_DEBUG_ASSERT(!m_SymbolTable.contains(node.name), "Failed to validate FlexShader function prototype | Symbol \"%s\" redefinition", node.name.c_str());
				
				m_SymbolTable[node.name] = node.returnType;

				return node.returnType;
			}

			Type ValidationVisitor::VisitFunction(Node::Function &node)
			{
				node.body->Accept(*this);

				return node.prototype->Accept(*this);
			}

			Type ValidationVisitor::VisitFunctionCall(Node::FunctionCall &node)
			{
				return GetType(node.name);
			}

			Type ValidationVisitor::VisitComponent(Node::Component &node)
			{
				node.body->Accept(*this);

				return Type::None;
			}

			Type ValidationVisitor::VisitVariableDeclaration(Node::VariableDeclaration &node)
			{
				GM_DEBUG_ASSERT(!m_SymbolTable.contains(node.name), "Failed to validate FlexShader variable declaration | Symbol \"%s\" redefinition", node.name.c_str());

				m_SymbolTable[node.name] = node.type;

				return node.type;
			}

			Type ValidationVisitor::VisitTypeDeclaration(Node::TypeDeclaration &node) { return Type::None; }

			Type ValidationVisitor::VisitNumericLiteral(Node::NumericLiteral &node)
			{
				//Assume that all numeric literals are floating point literals
				//Eventually need to add seperate token type for integer literals
				return Type::Float;
			}

			Type ValidationVisitor::VisitStringLiteral(Node::StringLiteral &node) { return Type::None; }

			Type ValidationVisitor::VisitIdentifier(Node::Identifier &node) { return GetType(node.name); }

			Type ValidationVisitor::VisitBinaryOperation(Node::BinaryOperation &node)
			{
				const Type leftType  = node.leftOperand->Accept(*this);
				const Type rightType = node.rightOperand->Accept(*this);

				GM_DEBUG_ASSERT(leftType == rightType, "Failed to validate FlexShader binary operation | Left operand type %s does not match right operand type %s", GetTypeString(leftType).c_str(), GetTypeString(rightType).c_str());
			
				return leftType;
			}

			Type ValidationVisitor::VisitAssignment(Node::Assignment &node)
			{
				const Type leftType  = node.lValue->Accept(*this);
				const Type rightType = node.rValue->Accept(*this);

				GM_DEBUG_ASSERT(leftType == rightType, "Failed to validate FlexShader assignment | Left type \"%s\" incompatible right type \"%s\"", GetTypeString(leftType).c_str(), GetTypeString(rightType).c_str());

				return leftType;
			}

			Type ValidationVisitor::GetType(const std::string &symbol)
			{
				GM_DEBUG_ASSERT(m_SymbolTable.contains(symbol), "Failed to get FlexShader type | Undefined symbol \"%s\"", symbol.c_str());

				return m_SymbolTable[symbol];
			}

			void ModuleVisitor::VisitStatementBlock(Node::StatementBlock &node)
			{
			}

			void ModuleVisitor::VisitFunctionPrototype(Node::FunctionPrototype &node)
			{
			}

			void ModuleVisitor::VisitFunction(Node::Function &node)
			{
			}

			void ModuleVisitor::VisitFunctionCall(Node::FunctionCall &node)
			{
			}

			void ModuleVisitor::VisitComponent(Node::Component &node)
			{
			}

			void ModuleVisitor::VisitVariableDeclaration(Node::VariableDeclaration &node)
			{
			}

			void ModuleVisitor::VisitNumericLiteral(Node::NumericLiteral &node)
			{
			}

			void ModuleVisitor::VisitStringLiteral(Node::StringLiteral &node)
			{
			}

			void ModuleVisitor::VisitIdentifier(Node::Identifier &node)
			{
			}

			void ModuleVisitor::VisitBinaryOperation(Node::BinaryOperation &node)
			{
			}

			void ModuleVisitor::VisitAssignment(Node::Assignment &node)
			{
			}
		}
	}
}