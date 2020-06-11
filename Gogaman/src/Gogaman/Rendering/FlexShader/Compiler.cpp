#include "pch.h"
#include "Compiler.h"

#include "Gogaman/Core/Base.h"

#include "Token.h"
#include "AbstractSyntaxTree.h"
#include "Lexer.h"
#include "Parser.h"

#define GM_FLEX_SHADER_SEMICOLON_PRECEDENCE 1
#define GM_FLEX_SHADER_COMMA_PRECEDENCE 5

//#define GM_FLEX_SHADER_ASSERT_TOKEN_TYPE(x) GM_DEBUG_ASSERT(parser.GetCurrentToken().type == Token::Type::x, "Failed to parse FlexShader | Invalid syntax | Token \"%s\" is invalid, expected %s", GetTokenString(parser.GetCurrentToken()).c_str(), GetTokenTypeString(Token::Type::x).c_str());

namespace Gogaman
{
	namespace FlexShader
	{
		AST::Node::Statement *ParseStatement(Parser &parser, const int8_t associativePrecedence, const Token &token, AST::Node::Abstract *leftNode)
		{
			AST::Node::Statement *node = static_cast<AST::Node::Statement *>(leftNode);

			//parser.AdvanceCursor();
			return node;
		}

		AST::Node::StatementBlock *ParseStatementBlock(Parser &parser, const int8_t associativePrecedence, const Token &token)
		{
			AST::Node::StatementBlock *node = new AST::Node::StatementBlock;

			while(parser.GetCurrentToken().type != Token::Type::RightBrace)
			{
				node->statements.emplace_back(static_cast<AST::Node::Statement *>(parser.Parse(associativePrecedence)));
				parser.AdvanceCursor();
			}

			GM_DEBUG_ASSERT(parser.GetCurrentToken().type == Token::Type::RightBrace, "Failed to parse FlexShader | Invalid syntax | Token \"%s\" is invalid, expected \"}\"", GetTokenString(parser.GetCurrentToken()).c_str());

			//Should it advance?
			//parser.AdvanceCursor();
			return node;
		}

		AST::Node::FunctionPrototype *ParseFunctionPrototype(Parser &parser, const int8_t associativePrecedence, const Token &token, AST::Node::Abstract *leftNode)
		{
			AST::Node::FunctionPrototype *node = new AST::Node::FunctionPrototype;
			//FunctionPrototype node should just store VariableDeclaration node, otherwise the leftNode here will never be deleted
			node->returnType = static_cast<AST::Node::VariableDeclaration *>(leftNode)->type;
			node->name       = static_cast<AST::Node::VariableDeclaration *>(leftNode)->name;

			//parser.AdvanceCursor();
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
				//node->arguments.emplace_back(static_cast<AST::Node::VariableDeclaration *>(parser.Parse(GM_FLEX_SHADER_COMMA_PRECEDENCE)));
				node->arguments.emplace_back(static_cast<AST::Node::VariableDeclaration *>(parser.Parse(associativePrecedence)));

				//parser.AdvanceCursor();
			}

			parser.AdvanceCursor();
			return node;
		}

		AST::Node::Function *ParseFunction(Parser &parser, const int8_t associativePrecedence, const Token &token, AST::Node::Abstract *leftNode)
		{
			AST::Node::Function *node = new AST::Node::Function;
			node->prototype = static_cast<AST::Node::FunctionPrototype *>(leftNode);
			//node->body      = static_cast<AST::Node::StatementBlock *>(parser.Parse(associativePrecedence));
			node->body = ParseStatementBlock(parser, 0, token);

			return node;
		}

		AST::Node::FunctionCall *ParseFunctionCall(Parser &parser, const int8_t associativePrecedence, const Token &token)
		{
			AST::Node::FunctionCall *node = new AST::Node::FunctionCall;
			node->name = token.lexeme;

			parser.AdvanceCursor();
			while(parser.GetCurrentToken().type != Token::Type::RightParenthesis)
			{
				node->arguments.emplace_back(static_cast<AST::Node::Expression *>(parser.Parse(GM_FLEX_SHADER_COMMA_PRECEDENCE)));

				if(parser.GetCurrentToken().type == Token::Type::Comma)
					parser.AdvanceCursor();
			}

			parser.AdvanceCursor();
			return node;
		}

		AST::Node::Component *ParseComponent(Parser &parser, const int8_t associativePrecedence, const Token &token)
		{
			AST::Node::Component *node = new AST::Node::Component;
			//GM_FLEX_SHADER_ASSERT_TOKEN_TYPE(Identifier);
			node->name = parser.GetCurrentToken().lexeme;

			parser.AdvanceCursor();

			node->body = static_cast<AST::Node::StatementBlock *>(parser.Parse(associativePrecedence));

			//parser.AdvanceCursor();
			return node;
		}

		AST::Node::VariableDeclaration *ParseVariableDeclaration(Parser &parser, const int8_t associativePrecedence, const Token &token)
		{
			AST::Node::VariableDeclaration *node = new AST::Node::VariableDeclaration;
			node->type = AST::GetType(std::string(token.lexeme));

			//Assert that it's an identifier here
			node->name = parser.GetCurrentToken().lexeme;

			//GM_DEBUG_ASSERT(parser.GetCurrentToken().type == Token::Type::Identifier, "Failed to parse FlexShader | Invalid syntax | Token \"%s\" is invalid, expected identifier", GetTokenString(parser.GetCurrentToken()).c_str());

			//Advance cursor? YES! function prototype relies on the token being left parenthesis
			parser.AdvanceCursor();
			return node;
		}

		AST::Node::BinaryOperation *ParseBinaryOperation(Parser &parser, const int8_t associativePrecedence, const Token &token, AST::Node::Abstract *leftNode)
		{
			AST::Node::BinaryOperation *node = new AST::Node::BinaryOperation;
			node->operatorType = token.type;
			node->leftOperand  = static_cast<AST::Node::Expression *>(leftNode);
			node->rightOperand = static_cast<AST::Node::Expression *>(parser.Parse(associativePrecedence));
			return node;
		}

		AST::Node::Assignment *ParseAssignment(Parser &parser, const int8_t associativePrecedence, const Token &token, AST::Node::Abstract *leftNode)
		{
			AST::Node::Assignment *node = new AST::Node::Assignment;
			node->operatorType = token.type;
			node->lValue       = static_cast<AST::Node::Identifier *>(leftNode);
			node->rValue       = static_cast<AST::Node::Expression *>(parser.Parse(associativePrecedence));

			//Assert that current token is semicolon
			//parser.AdvanceCursor();
			return node;
		}

		AST::Node::Expression *ParseExpression(Parser &parser, const int8_t associativePrecedence, const Token &token)
		{
			AST::Node::Expression *node = static_cast<AST::Node::Expression *>(parser.Parse(associativePrecedence));

			GM_DEBUG_ASSERT(parser.GetCurrentToken().type == Token::Type::RightParenthesis, "Failed to parse FlexShader | Invalid syntax | Token \"%s\" is invalid, expected \")\"", GetTokenString(parser.GetCurrentToken()).c_str());

			parser.AdvanceCursor();
			return node;
		}

		AST::Node::TypeDeclaration *ParseTypeDeclaration(Parser &parser, const int8_t associativePrecedence, const Token &token)
		{
			AST::Node::TypeDeclaration *node = new AST::Node::TypeDeclaration;
			node->typeName = token.lexeme;

			GM_DEBUG_ASSERT(parser.GetCurrentToken().type == Token::Type::Identifier, "Failed to parse FlexShader | Invalid syntax | Token \"%s\" is invalid, expected identifier", GetTokenString(parser.GetCurrentToken()).c_str());

			return node;
		}

		AST::Node::NumericLiteral *ParseNumericLiteral(Parser &parser, const int8_t associativePrecedence, const Token &token)
		{
			AST::Node::NumericLiteral *node = new AST::Node::NumericLiteral;
			node->value = token.lexeme;
			return node;
		}

		AST::Node::StringLiteral *ParseStringLiteral(Parser &parser, const int8_t associativePrecedence, const Token &token)
		{
			AST::Node::StringLiteral *node = new AST::Node::StringLiteral;
			node->value = token.lexeme;
			return node;
		}

		AST::Node::Identifier *ParseIdentifier(Parser &parser, const int8_t associativePrecedence, const Token &token)
		{
			AST::Node::Identifier *node = new AST::Node::Identifier;
			node->name = token.lexeme;
			return node;
		}

		AST::Node::Abstract *Compile(const std::string &source)
		{
			std::vector<Token> tokens = Tokenize(source);

			//for(auto &i : tokens)
				//GM_LOG_CORE_INFO("Token: %s", GetTokenString(i).c_str());

			Parser parser(std::move(tokens));

			//parser.AddLeftTokenTypes(31, ParseVariableDeclaration, { Token::Type::Identifier });

			//parser.AddLeftTokenTypes(31, ParseFunctionCall, { Token::Type::LeftParenthesis });
			parser.AddLeftTokenTypes(31, ParseFunctionPrototype, { Token::Type::LeftParenthesis });
			
			parser.AddLeftTokenTypes(31, ParseFunction, { Token::Type::LeftBrace });

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
				//Multiplication, division, modulo
				parser.AddLeftTokenTypes(25, ParseBinaryOperation, { Token::Type::Asterisk, Token::Type::Slash, Token::Type::Percent });
				//Addition, subtraction
				parser.AddLeftTokenTypes(23, ParseBinaryOperation, { Token::Type::Plus, Token::Type::Minus });
			}

			//Boolean operators
			//== != <= >= < > && ||

			//Assignment
			//= += -= *= /= %=
			parser.AddLeftTokenTypes(6, ParseAssignment, { Token::Type::Equal });
			/*
			parser.AddConditionalNullTokenTypes(-1, [](Parser &parser, const Token &token)
				{
					const std::vector<std::string> typeNames = { "int", "float" };
					for(const std::string &i : typeNames)
					{
						if(token.lexeme == i)
							return true;
					}

					return false;
				}, ParseTypeDeclaration, { Token::Type::Identifier });
			*/

			parser.AddNullTokenTypes(-1, ParseNumericLiteral, { Token::Type::Number });
			parser.AddNullTokenTypes(-1, ParseStringLiteral,  { Token::Type::String });
			parser.AddNullTokenTypes(-1, ParseIdentifier, { Token::Type::Identifier });
			//ParseKeyword, if needed?

			parser.AddConditionalNullTokenTypes(-1, [](Parser &parser, const Token &token)
				{
					const std::vector<std::string> typeNames = { "int", "float" };
					for(const std::string &i : typeNames)
						if(token.lexeme == i)
							return true;

					return false;
				}, ParseVariableDeclaration, { Token::Type::Keyword });

			parser.AddConditionalNullTokenTypes(31, [](Parser &parser, const Token &token)
				{
					return token.lexeme == "component";
				}, ParseComponent, { Token::Type::Keyword });

			AST::Node::Abstract *rootNode = parser.Parse();

			AST::PrintVisitor printer;
			rootNode->Accept(printer);

			AST::ValidationVisitor validator;
			rootNode->Accept(validator);

			const auto &symbols0 = validator.GetSymbolTable(0);
			for(auto i = symbols0.begin(); i != symbols0.end(); i++)
			{
				GM_LOG_CORE_INFO("Global namespace | Symbol | Name: %s | Type: %s", i->first.c_str(), GetTypeString(i->second).c_str());
			}

			const auto &symbols1 = validator.GetSymbolTable(1);
			for(auto i = symbols1.begin(); i != symbols1.end(); i++)
			{
				GM_LOG_CORE_INFO("Component namespace | Symbol | Name: %s | Type: %s", i->first.c_str(), GetTypeString(i->second).c_str());
			}

			const auto &symbols2 = validator.GetSymbolTable(2);
			for(auto i = symbols2.begin(); i != symbols2.end(); i++)
			{
				GM_LOG_CORE_INFO("Function namespace | Symbol | Name: %s | Type: %s", i->first.c_str(), GetTypeString(i->second).c_str());
			}

			return rootNode;
		}
	}
}