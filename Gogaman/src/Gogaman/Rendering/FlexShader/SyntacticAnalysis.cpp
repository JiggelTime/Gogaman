#include "pch.h"
#include "SyntacticAnalysis.h"

#include "Gogaman/Core/Base.h"

#include "Token.h"

#include "AbstractSyntaxTree.h"
#include "Parser.h"

#define GM_FLEX_SHADER_SEMICOLON_BINDING_POWER 1
#define GM_FLEX_SHADER_COMMA_BINDING_POWER     2

//#define GM_FLEX_SHADER_ASSERT_TOKEN_TYPE(x) GM_DEBUG_ASSERT(parser.GetCurrentToken().type == Token::Type::x, "Failed to parse FlexShader | Invalid syntax | Token \"%s\" is invalid, expected %s", GetTokenString(parser.GetCurrentToken()).c_str(), GetTokenTypeString(Token::Type::x).c_str());

namespace Gogaman
{
	namespace FlexShader
	{
		AST::Node::Statement *ParseStatement(Parser &parser, const int8_t bindingPower, const Token &token, AST::Node::Abstract *leftNode)
		{
			AST::Node::Statement *node = static_cast<AST::Node::Statement *>(leftNode);
			return node;
		}

		AST::Node::StatementBlock *ParseStatementBlock(Parser &parser, const int8_t bindingPower, const Token &token)
		{
			AST::Node::StatementBlock *node = new AST::Node::StatementBlock;

			while(parser.GetCurrentToken().type != Token::Type::RightBrace)
			{
				node->statements.emplace_back(static_cast<AST::Node::Statement *>(parser.Parse(bindingPower)));
				parser.AdvanceCursor();
			}

			GM_DEBUG_ASSERT(parser.GetCurrentToken().type == Token::Type::RightBrace, "Failed to parse FlexShader | Invalid syntax | Token \"%s\" is invalid, expected \"}\"", GetTokenString(parser.GetCurrentToken()).c_str());
			return node;
		}

		AST::Node::FunctionPrototype *ParseFunctionPrototype(Parser &parser, const int8_t bindingPower, const Token &token, AST::Node::Abstract *leftNode)
		{
			AST::Node::FunctionPrototype *node = new AST::Node::FunctionPrototype;
			node->returnType = static_cast<AST::Node::VariableDeclaration *>(leftNode)->type;
			node->name       = static_cast<AST::Node::VariableDeclaration *>(leftNode)->name;
			//delete leftNode

			while(parser.GetCurrentToken().type != Token::Type::RightParenthesis)
			{
				if(parser.GetCurrentToken().type == Token::Type::Comma)
				{
					parser.AdvanceCursor();
					//Assert that current token is not a comma here
					continue;
				}

				//GM_DEBUG_ASSERT(parser.GetCurrentToken().type == Token::Type::Identifier, "Failed to parse FlexShader | Invalid syntax | Token \"%s\" is invalid, expected identifier", GetTokenString(parser.GetCurrentToken()).c_str());

				//It probably should be this:
				//node->parameters.emplace_back(static_cast<AST::Node::VariableDeclaration *>(parser.Parse(GM_FLEX_SHADER_COMMA_BINDING_POWER)));
				node->parameters.emplace_back(static_cast<AST::Node::VariableDeclaration *>(parser.Parse(bindingPower)));

				//parser.AdvanceCursor();
			}

			parser.AdvanceCursor();
			return node;
		}

		AST::Node::Function *ParseFunction(Parser &parser, const int8_t bindingPower, const Token &token, AST::Node::Abstract *leftNode)
		{
			AST::Node::Function *node = new AST::Node::Function;
			node->prototype = static_cast<AST::Node::FunctionPrototype *>(leftNode);
			//node->body      = static_cast<AST::Node::StatementBlock *>(parser.Parse(bindingPower));
			node->body = ParseStatementBlock(parser, 0, token);

			return node;
		}

		AST::Node::FunctionCall *ParseFunctionCall(Parser &parser, const int8_t bindingPower, const Token &token)
		{
			AST::Node::FunctionCall *node = new AST::Node::FunctionCall;
			node->name = token.lexeme;

			parser.AdvanceCursor();
			while(parser.GetCurrentToken().type != Token::Type::RightParenthesis)
			{
				node->parameters.emplace_back(static_cast<AST::Node::Expression *>(parser.Parse(GM_FLEX_SHADER_COMMA_BINDING_POWER)));

				if(parser.GetCurrentToken().type == Token::Type::Comma)
					parser.AdvanceCursor();
			}

			parser.AdvanceCursor();
			return node;
		}

		AST::Node::Component *ParseComponent(Parser &parser, const int8_t bindingPower, const Token &token)
		{
			AST::Node::Component *node = new AST::Node::Component;
			//GM_FLEX_SHADER_ASSERT_TOKEN_TYPE(Identifier);
			node->name = parser.GetCurrentToken().lexeme;

			parser.AdvanceCursor();

			if(parser.GetCurrentToken().type == Token::Type::Colon)
			{
				parser.AdvanceCursor();
				node->interfaceName = parser.GetCurrentToken().lexeme;
				g_ParsedComponents.emplace(node->interfaceName);
				parser.AdvanceCursor();
			}

			node->body = static_cast<AST::Node::StatementBlock *>(parser.Parse(bindingPower));

			g_ParsedComponents.emplace(node->name);

			//parser.AdvanceCursor();
			return node;
		}

		AST::Node::VariableDeclaration *ParseVariableDeclaration(Parser &parser, const int8_t bindingPower, const Token &token)
		{
			AST::Node::VariableDeclaration *node = new AST::Node::VariableDeclaration;

			if(GetVariableSpecifier(std::string(token.lexeme)) != VariableSpecifier::None)
			{
				node->specifier = GetVariableSpecifier(std::string(token.lexeme));
				node->type      = GetType(std::string(parser.GetCurrentToken().lexeme));
				parser.AdvanceCursor();
			}
			else
			{
				node->specifier = VariableSpecifier::None;
				node->type      = GetType(std::string(token.lexeme));
			}

			//Assert that it's an identifier here
			node->name = parser.GetCurrentToken().lexeme;

			//GM_DEBUG_ASSERT(parser.GetCurrentToken().type == Token::Type::Identifier, "Failed to parse FlexShader | Invalid syntax | Token \"%s\" is invalid, expected identifier", GetTokenString(parser.GetCurrentToken()).c_str());

			//Advance cursor? YES! function prototype relies on the token being left parenthesis
			parser.AdvanceCursor();
			return node;
		}

		AST::Node::ComponentInstantiation *ParseComponentInstantiation(Parser &parser, const int8_t bindingPower, const Token &token)
		{
			AST::Node::ComponentInstantiation *node = new AST::Node::ComponentInstantiation;
			node->typeName = token.lexeme;

			//Assert that it's an identifier here
			node->name = parser.GetCurrentToken().lexeme;

			//GM_DEBUG_ASSERT(parser.GetCurrentToken().type == Token::Type::Identifier, "Failed to parse FlexShader | Invalid syntax | Token \"%s\" is invalid, expected identifier", GetTokenString(parser.GetCurrentToken()).c_str());

			//Advance cursor? YES! function prototype relies on the token being left parenthesis
			parser.AdvanceCursor();
			return node;
		}

		AST::Node::Vector *ParseVector(Parser &parser, const int8_t bindingPower, const Token &token, AST::Node::Abstract *leftNode)
		{
			AST::Node::Vector *node = new AST::Node::Vector;
			node->expressions.emplace_back(static_cast<AST::Node::Expression *>(leftNode));
			while(true)
			{
				node->expressions.emplace_back(static_cast<AST::Node::Expression *>(parser.Parse(bindingPower)));

				if(parser.GetCurrentToken().type != Token::Type::Comma)
					break;

				parser.AdvanceCursor();
			}

			//GM_DEBUG_ASSERT(parser.GetCurrentToken().type == Token::Type::RightBrace, "Failed to parse FlexShader | Invalid syntax | Token \"%s\" is invalid, expected \"}\"", GetTokenString(parser.GetCurrentToken()).c_str());
			return node;
		}

		AST::Node::MemberSelection *ParseMemberSelection(Parser &parser, const uint8_t bindingPower, const Token &token, AST::Node::Abstract *leftNode)
		{
			AST::Node::MemberSelection *node = new AST::Node::MemberSelection;
			node->object     = static_cast<AST::Node::Identifier *>(leftNode);
			node->memberName = parser.GetCurrentToken().lexeme;
			parser.AdvanceCursor();
			return node;
		}

		AST::Node::BinaryOperation *ParseBinaryOperation(Parser &parser, const int8_t bindingPower, const Token &token, AST::Node::Abstract *leftNode)
		{
			AST::Node::BinaryOperation *node = new AST::Node::BinaryOperation;
			node->operatorType = token.type;
			node->leftOperand  = static_cast<AST::Node::Expression *>(leftNode);
			node->rightOperand = static_cast<AST::Node::Expression *>(parser.Parse(bindingPower));
			return node;
		}

		AST::Node::Assignment *ParseAssignment(Parser &parser, const int8_t bindingPower, const Token &token, AST::Node::Abstract *leftNode)
		{
			AST::Node::Assignment *node = new AST::Node::Assignment;
			node->operatorType = token.type;
			node->lValue       = static_cast<AST::Node::Identifier *>(leftNode);
			node->rValue       = static_cast<AST::Node::Expression *>(parser.Parse(bindingPower));

			//Assert that current token is semicolon
			//parser.AdvanceCursor();
			return node;
		}

		AST::Node::Branch *ParseBranch(Parser &parser, const int8_t bindingPower, const Token &token)
		{
			AST::Node::Branch *node = new AST::Node::Branch;
			node->condition = static_cast<AST::Node::Expression *>(parser.Parse(bindingPower));
			node->ifBody = static_cast<AST::Node::Statement *>(parser.Parse(0));
			if(parser.Peek().lexeme == "else")
			{
				parser.AdvanceCursor();
				parser.AdvanceCursor();
				node->elseBody = static_cast<AST::Node::Statement *>(parser.Parse(0));
			}

			return node;
		}

		AST::Node::Return *ParseReturn(Parser &parser, const int8_t bindingPower, const Token &token)
		{
			AST::Node::Return *node = new AST::Node::Return;
			if(parser.GetCurrentToken().type != Token::Type::Semicolon)
				node->value = static_cast<AST::Node::Expression *>(parser.Parse(bindingPower));
			else
				node->value = nullptr;
			return node;
		}

		AST::Node::Expression *ParseExpression(Parser &parser, const int8_t bindingPower, const Token &token)
		{
			AST::Node::Expression *node = static_cast<AST::Node::Expression *>(parser.Parse(bindingPower));

			GM_DEBUG_ASSERT(parser.GetCurrentToken().type == Token::Type::RightParenthesis, "Failed to parse FlexShader | Invalid syntax | Token \"%s\" is invalid, expected \")\"", GetTokenString(parser.GetCurrentToken()).c_str());

			parser.AdvanceCursor();
			return node;
		}

		AST::Node::BooleanLiteral *ParseBooleanLiteral(Parser &parser, const int8_t bindingPower, const Token &token)
		{
			AST::Node::BooleanLiteral *node = new AST::Node::BooleanLiteral;
			node->value = token.lexeme == "true" ? true : false;
			return node;
		}

		AST::Node::IntegerLiteral *ParseIntegerLiteral(Parser &parser, const int8_t bindingPower, const Token &token)
		{
			AST::Node::IntegerLiteral *node = new AST::Node::IntegerLiteral;
			node->value = strtoul(std::string(token.lexeme).c_str(), nullptr, 10);
			return node;
		}

		AST::Node::FloatingPointLiteral *ParseFloatingPointLiteral(Parser &parser, const int8_t bindingPower, const Token &token)
		{
			AST::Node::FloatingPointLiteral *node = new AST::Node::FloatingPointLiteral;
			node->value = atof(std::string(token.lexeme).c_str());
			return node;
		}

		AST::Node::StringLiteral *ParseStringLiteral(Parser &parser, const int8_t bindingPower, const Token &token)
		{
			AST::Node::StringLiteral *node = new AST::Node::StringLiteral;
			node->value = token.lexeme;
			return node;
		}

		AST::Node::Identifier *ParseIdentifier(Parser &parser, const int8_t bindingPower, const Token &token)
		{
			AST::Node::Identifier *node = new AST::Node::Identifier;
			node->name = token.lexeme;
			return node;
		}

		AST::Node::Abstract *Parse(std::vector<Token> &&tokens)
		{
			Parser parser(std::move(tokens));

			//This used to have a binding power of 6 for some reason. It turned multiple function call parameters into a vector
			parser.AddLeftTokenTypes(2, ParseVector, { Token::Type::Comma });

			parser.AddLeftTokenTypes(7, ParseMemberSelection, { Token::Type::Dot });

			parser.AddLeftTokenTypes(6, ParseFunctionPrototype, { Token::Type::LeftParenthesis });
			
			parser.AddLeftTokenTypes(6, ParseFunction, { Token::Type::LeftBrace });

			parser.AddConditionalNullTokenTypes(-1, [](Parser &parser, const Token &token)
				{
					return parser.GetCurrentToken().type == Token::Type::LeftParenthesis;
				}, ParseFunctionCall, { Token::Type::Identifier });

			parser.AddNullTokenTypes(0, ParseStatementBlock, { Token::Type::LeftBrace });
			parser.AddNullTokenTypes(-1, NullParseError, { Token::Type::RightBrace });

			parser.AddLeftTokenTypes(0, ParseStatement, { Token::Type::Semicolon });

			parser.AddNullTokenTypes(0, ParseExpression, { Token::Type::LeftParenthesis });
			parser.AddNullTokenTypes(-1, NullParseError, { Token::Type::RightParenthesis });

			//Arithmetic operators
			{
				//Addition, subtraction
				parser.AddLeftTokenTypes(4, ParseBinaryOperation, { Token::Type::Plus, Token::Type::Minus });
				//Multiplication, division, modulo
				parser.AddLeftTokenTypes(5, ParseBinaryOperation, { Token::Type::Asterisk, Token::Type::Slash, Token::Type::Percent });
			}

			//Boolean operators
			//== != <= >= < > && ||

			//Assignment
			//= += -= *= /= %=
			parser.AddLeftTokenTypes(3, ParseAssignment, { Token::Type::Equal });

			parser.AddNullTokenTypes(-1, ParseBooleanLiteral,       { Token::Type::BooleanLiteral });
			parser.AddNullTokenTypes(-1, ParseIntegerLiteral,       { Token::Type::IntegerLiteral });
			parser.AddNullTokenTypes(-1, ParseFloatingPointLiteral, { Token::Type::FloatingPointLiteral });
			parser.AddNullTokenTypes(-1, ParseStringLiteral,        { Token::Type::StringLiteral });
			parser.AddNullTokenTypes(-1, ParseIdentifier,           { Token::Type::Identifier });

			parser.AddConditionalNullTokenTypes(-1, [](Parser &parser, const Token &token)
				{
					return IsTypeName(std::string(token.lexeme)) || (token.lexeme == "input") || (token.lexeme == "output");
				}, ParseVariableDeclaration, { Token::Type::Keyword });

			parser.AddConditionalNullTokenTypes(-1, [](Parser &parser, const Token &token)
				{
					return g_ParsedComponents.contains(std::string(token.lexeme));
				}, ParseComponentInstantiation, { Token::Type::Identifier });

			parser.AddConditionalNullTokenTypes(6, [](Parser &parser, const Token &token)
				{
					return token.lexeme == "component";
				}, ParseComponent, { Token::Type::Keyword });

			parser.AddConditionalNullTokenTypes(6, [](Parser &parser, const Token &token)
				{
					return token.lexeme == "if";
				}, ParseBranch, { Token::Type::Keyword });
			
			parser.AddConditionalNullTokenTypes(3, [](Parser &parser, const Token &token)
				{
					return token.lexeme == "return";
				}, ParseReturn, { Token::Type::Keyword });

			return parser.Parse();
		}
	}
}