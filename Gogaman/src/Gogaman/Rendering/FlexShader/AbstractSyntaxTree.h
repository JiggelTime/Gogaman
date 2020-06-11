#pragma once

#include "Gogaman/Core/Base.h"

#include "Token.h"

#include "Visitor.h"

#define GM_AST_NODE_ACCEPT_FUNCTIONS(x)\
inline virtual void Accept(PrintVisitor &visitor) { visitor.Visit##x(*this); }\
inline virtual Type Accept(ValidationVisitor &visitor) { return visitor.Visit##x(*this); }\
inline virtual void Accept(ModuleVisitor &visitor) { visitor.Visit##x(*this); }

namespace Gogaman
{
	namespace FlexShader
	{
		namespace AST
		{
			/*
			struct Node
			{
				enum class Type : uint8_t
				{
					Add,
					Subtract,
					Multiply,
					Divide,
					Modulo,
					Expression
				};

				std::vector<Node *> children;
				Token               token;
				Type                type;
			};
			*/

			enum class Type : uint8_t
			{
				None,
				Integer,
				Float
			};

			inline Type GetType(const std::string &typeString)
			{
				if(typeString == "int")
					return Type::Integer;
				else if(typeString == "float")
					return Type::Float;
				else
					GM_DEBUG_ASSERT(false, "Failed to get FlexShader type | Invalid type string");
			}

			inline std::string GetTypeString(const Type type)
			{
				const std::string names[]
				{
				"none",
				"int",
				"float"
				};

				return std::string(names[(uint8_t)type]);
			}

			namespace Node
			{
				struct Abstract
				{
					GM_AST_NODE_ACCEPT_FUNCTIONS(Abstract)

					//virtual ~Abstract() = default;
				};

				struct Expression : public Abstract
				{
					GM_AST_NODE_ACCEPT_FUNCTIONS(Expression)
				};

				struct Statement : public Abstract
				{
					GM_AST_NODE_ACCEPT_FUNCTIONS(Statement)
				};

				struct StatementBlock : public Statement
				{
					GM_AST_NODE_ACCEPT_FUNCTIONS(StatementBlock)

					std::vector<Statement *> statements;
				};

				struct VariableDeclaration : public Statement
				{
					GM_AST_NODE_ACCEPT_FUNCTIONS(VariableDeclaration)
					
					std::string name;
					Type        type;
				};

				struct FunctionPrototype : public Statement
				{
					GM_AST_NODE_ACCEPT_FUNCTIONS(FunctionPrototype)

					std::string                        name;
					Type                               returnType;
					//std::vector<std::string> argumentNames;
					std::vector<VariableDeclaration *> arguments;
				};

				struct Function : public Statement
				{
					GM_AST_NODE_ACCEPT_FUNCTIONS(Function)
					
					FunctionPrototype *prototype;
					StatementBlock    *body;
				};

				struct FunctionCall : public Expression
				{
					GM_AST_NODE_ACCEPT_FUNCTIONS(FunctionCall)

					std::vector<Expression *> arguments;
					std::string               name;
				};

				struct Component : public Statement
				{
					GM_AST_NODE_ACCEPT_FUNCTIONS(Component)

					std::string     name;
					StatementBlock *body;
				};

				struct TypeDeclaration : public Expression
				{
					GM_AST_NODE_ACCEPT_FUNCTIONS(TypeDeclaration)

					std::string typeName;
				};

				struct NumericLiteral : public Expression
				{
					GM_AST_NODE_ACCEPT_FUNCTIONS(NumericLiteral)

					std::string value;
				};

				struct StringLiteral : public Expression
				{
					GM_AST_NODE_ACCEPT_FUNCTIONS(StringLiteral)

					std::string value;
				};

				struct Identifier : public Expression
				{
					GM_AST_NODE_ACCEPT_FUNCTIONS(Identifier)

					std::string name;
				};

				struct BinaryOperation : public Expression
				{
					GM_AST_NODE_ACCEPT_FUNCTIONS(BinaryOperation)

					//Unique_ptr?
					Expression  *leftOperand, *rightOperand;
					Token::Type operatorType;
					//Token::Type operator;
				};

				struct Assignment : public Statement
				{
					GM_AST_NODE_ACCEPT_FUNCTIONS(Assignment)

					//Lvalue lValue;
					Identifier *lValue;
					Expression *rValue;
					//TODO: AssignmentOperator enum instead of Token::Type, eg. = += -=
					Token::Type operatorType;
				};
			}
		}
	}
}