#pragma once

#include "Gogaman/Core/Base.h"

#include "Token.h"

#include "Type.h"

#include "Visitor.h"
#include "SemanticAnalyzer.h"
#include "Translator.h"

#define GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(x)\
inline virtual void Accept(LogVisitor &visitor) { visitor.Visit##x(*this); }\
inline virtual Abstract *Accept(ModuleVisitor &visitor, const std::string &componentName = "") { return visitor.Visit##x(*this, componentName); }\
inline virtual void Accept(SpecializationVisitor &visitor) { visitor.Visit##x(*this); }\
inline virtual Type Accept(SemanticAnalyzer &visitor) { return visitor.Visit##x(*this); }\
inline virtual IR::Address Accept(Translator &visitor) { return visitor.Visit##x(*this); }

namespace Gogaman
{
	namespace FlexShader
	{
		namespace AST
		{
			namespace Node
			{
				struct Abstract
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(Abstract)

					//virtual ~Abstract() = default;
				};

				struct Expression : public Abstract
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(Expression)
				};

				struct Statement : public Abstract
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(Statement)
				};

				struct StatementBlock : public Statement
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(StatementBlock)

					std::vector<Statement *> statements;
				};

				struct VariableDeclaration : public Statement
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(VariableDeclaration)
					
					std::string name;
					Type        type;
					uint8_t     specifierFlags;
				};

				struct ComponentInstantiation : public Statement
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(ComponentInstantiation)
					
					std::string name, typeName;
				};

				struct FunctionPrototype : public Statement
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(FunctionPrototype)

					std::string                        name;
					Type                               returnType;
					//std::vector<std::string> parameterNames;
					std::vector<VariableDeclaration *> parameters;
				};

				struct Function : public Statement
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(Function)
					
					FunctionPrototype *prototype;
					StatementBlock    *body;
				};

				struct FunctionCall : public Expression
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(FunctionCall)

					std::vector<Expression *> parameters;
					std::string               name;
				};

				struct Component : public Statement
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(Component)

					std::string     name, interfaceName = "";
					StatementBlock *body;
				};

				struct NumericLiteral : public Expression
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(NumericLiteral)

					std::string value;
				};

				struct StringLiteral : public Expression
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(StringLiteral)

					std::string value;
				};

				struct Identifier : public Expression
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(Identifier)

					std::string name;
				};

				struct BinaryOperation : public Expression
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(BinaryOperation)

					//Unique_ptr?
					Expression  *leftOperand, *rightOperand;
					Token::Type operatorType;
					//Token::Type operator;
				};

				struct Assignment : public Statement
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(Assignment)

					//Lvalue lValue;
					Identifier *lValue;
					Expression *rValue;
					//TODO: AssignmentOperator enum instead of Token::Type, eg. = += -=
					Token::Type operatorType;
				};

				struct Branch : public Statement
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(Branch)
					
					Expression *condition;
					Statement *ifBody, *elseBody = nullptr;
				};

				struct Return : public Statement
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(Return)

					Expression *value;
				};
			}
		}
	}
}