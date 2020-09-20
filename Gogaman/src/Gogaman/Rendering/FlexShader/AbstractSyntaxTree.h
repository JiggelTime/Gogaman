#pragma once

#include "Gogaman/Core/Base.h"

#include "Token.h"

#include "Type.h"

#include "Visitor.h"
#include "Transformer.h"
#include "SemanticAnalyzer.h"
#include "Translator.h"

#define GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(x)\
inline virtual void Accept(LogVisitor &visitor) { visitor.Visit##x(*this); }\
inline virtual void Accept(SpecializationVisitor &visitor) { visitor.Visit##x(*this); }\
inline virtual std::vector<Abstract *> Accept(Transformer &visitor, const std::string &componentName = "") { return visitor.Visit##x(*this, componentName); }\
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
					
					std::string       name;
					Type              type;
					VariableSpecifier specifier;
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

				struct BooleanLiteral : public Expression
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(BooleanLiteral)

					bool value;
				};

				struct IntegerLiteral : public Expression
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(IntegerLiteral)

					uint32_t value;
				};

				struct FloatingPointLiteral : public Expression
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(FloatingPointLiteral)

					float value;
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

				struct Vector : public Expression
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(Vector)

					std::vector<Expression *> expressions;
				};

				struct MemberSelection : public Expression
				{
					GM_FLEX_SHADER_AST_NODE_ACCEPT_FUNCTIONS(MemberSelection)

					std::string memberName;
					Identifier  *object;
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