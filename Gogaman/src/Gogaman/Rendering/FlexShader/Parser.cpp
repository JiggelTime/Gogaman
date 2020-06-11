#include "pch.h"
#include "Parser.h"

#include "Gogaman/Core/Base.h"

#define GM_FLEX_SHADER_INVALID_ASSOCIATIVE_PRECEDENCE INT8_MAX

namespace Gogaman
{
	namespace FlexShader
	{
		using NullTokenParseFunction = std::function<AST::Node::Abstract *(Parser &, const int8_t, const Token &)>;
		using LeftTokenParseFunction = std::function<AST::Node::Abstract *(Parser &, const int8_t, const Token &, AST::Node::Abstract *)>;

		AST::Node::Abstract *NullParseError(Parser &parser, const int8_t associativePrecedence, const Token &token)
		{
			GM_DEBUG_ASSERT(false, "Failed to parse FlexShader | Invalid syntax | Line: %d | Token \"%s\" is invalid", token.line, GetTokenString(token).c_str());
		}

		AST::Node::Abstract *LeftParseError(Parser &parser, const int8_t rightAssociativePrecedence, const Token &token, AST::Node::Abstract *leftNode)
		{
			GM_DEBUG_ASSERT(false, "Failed to parse FlexShader | Invalid syntax | Line: %d | Token \"%s\" is invalid", token.line, GetTokenString(token).c_str());
		}

		const Token &Parser::GetCurrentToken() const
		{
			GM_DEBUG_ASSERT(m_CursorPosition < m_Tokens.size(), "Failed to get current token | End of tokens reached");

			return m_Tokens[m_CursorPosition];
		}

		Parser::Parser(std::vector<Token> &&tokens)
			: m_Tokens(std::move(tokens)), m_CursorPosition(0)
		{
			for(uint8_t i = 0; i < GM_FLEX_SHADER_TOKEN_TYPE_COUNT; i++)
			{
				//m_NullTokenAssociativePrecedences[i]     = GM_FLEX_SHADER_INVALID_ASSOCIATIVE_PRECEDENCE;
				//m_LeftTokenLeftAssociativePrecedences[i] = GM_FLEX_SHADER_INVALID_ASSOCIATIVE_PRECEDENCE;
				m_NullTokenAssociativePrecedences[i]     = 0;
				m_LeftTokenLeftAssociativePrecedences[i] = 0;

				m_NullTokenParseCallbacks[i] = NullParseError;
				m_LeftTokenParseCallbacks[i] = LeftParseError;
			}
		}

		void Parser::AddNullTokenTypes(const int8_t associativePrecedence, const NullTokenParseFunction &parseCallback, const std::vector<Token::Type> &tokenTypes)
		{
			GM_DEBUG_ASSERT(associativePrecedence != GM_FLEX_SHADER_INVALID_ASSOCIATIVE_PRECEDENCE, "Failed to add null token types | Invalid associative precedence");

			for(const Token::Type i : tokenTypes)
			{
				m_NullTokenParseCallbacks[(uint8_t)i]         = parseCallback;
				m_NullTokenAssociativePrecedences[(uint8_t)i] = associativePrecedence;

				if(m_LeftTokenLeftAssociativePrecedences[(uint8_t)i] == GM_FLEX_SHADER_INVALID_ASSOCIATIVE_PRECEDENCE)
				{
					m_LeftTokenParseCallbacks[(uint8_t)i]              = LeftParseError;
					m_LeftTokenLeftAssociativePrecedences[(uint8_t)i]  = 0;
					m_LeftTokenRightAssociativePrecedences[(uint8_t)i] = 0;
				}
			}
		}

		void Parser::AddLeftTokenTypes(const int8_t associativePrecedence, const LeftTokenParseFunction &parseCallback, const std::vector<Token::Type> &tokenTypes)
		{
			GM_DEBUG_ASSERT(associativePrecedence != GM_FLEX_SHADER_INVALID_ASSOCIATIVE_PRECEDENCE, "Failed to add left token types | Invalid associative precedence");

			for(const Token::Type i : tokenTypes)
			{
				m_LeftTokenParseCallbacks[(uint8_t)i]              = parseCallback;
				m_LeftTokenLeftAssociativePrecedences[(uint8_t)i]  = associativePrecedence;
				m_LeftTokenRightAssociativePrecedences[(uint8_t)i] = associativePrecedence;

				if(m_NullTokenAssociativePrecedences[(uint8_t)i] == GM_FLEX_SHADER_INVALID_ASSOCIATIVE_PRECEDENCE)
				{
					m_NullTokenParseCallbacks[(uint8_t)i]         = NullParseError;
					m_NullTokenAssociativePrecedences[(uint8_t)i] = 0;
				}
			}
		}

		void Parser::AddRightTokenTypes(const int8_t associativePrecedence, const LeftTokenParseFunction &parseCallback, const std::vector<Token::Type> &tokenTypes)
		{
			GM_DEBUG_ASSERT(associativePrecedence != GM_FLEX_SHADER_INVALID_ASSOCIATIVE_PRECEDENCE, "Failed to add right token types | Invalid associative precedence");

			for(const Token::Type i : tokenTypes)
			{
				m_LeftTokenParseCallbacks[(uint8_t)i]              = parseCallback;
				m_LeftTokenLeftAssociativePrecedences[(uint8_t)i]  = associativePrecedence;
				m_LeftTokenRightAssociativePrecedences[(uint8_t)i] = associativePrecedence - 1;

				if(m_NullTokenAssociativePrecedences[(uint8_t)i] == GM_FLEX_SHADER_INVALID_ASSOCIATIVE_PRECEDENCE)
				{
					m_NullTokenParseCallbacks[(uint8_t)i]         = NullParseError;
					m_NullTokenAssociativePrecedences[(uint8_t)i] = 0;
				}
			}
		}

		void Parser::AddConditionalNullTokenTypes(const int8_t associativePrecedence, const NullTokenParseConditionFunction &parseCondition, const NullTokenParseFunction &parseCallback, const std::vector<Token::Type> &tokenTypes)
		{
			GM_DEBUG_ASSERT(associativePrecedence != GM_FLEX_SHADER_INVALID_ASSOCIATIVE_PRECEDENCE, "Failed to add conditional null token types | Invalid associative precedence");

			for(const Token::Type i : tokenTypes)
			{
				m_NullTokenConditionalParseConditions[(uint8_t)i].emplace_back(parseCondition);
				m_NullTokenConditionalParseCallbacks[(uint8_t)i].emplace_back(parseCallback);
				m_NullTokenAssociativePrecedences[(uint8_t)i] = associativePrecedence;

				//figure this out. new
				if(m_LeftTokenLeftAssociativePrecedences[(uint8_t)i] == GM_FLEX_SHADER_INVALID_ASSOCIATIVE_PRECEDENCE)
				{
					m_LeftTokenParseCallbacks[(uint8_t)i] = LeftParseError;
					m_LeftTokenLeftAssociativePrecedences[(uint8_t)i] = 0;
					m_LeftTokenRightAssociativePrecedences[(uint8_t)i] = 0;
				}
			}
		}

		void Parser::AddConditionalLeftTokenTypes(const int8_t associativePrecedence, const LeftTokenParseConditionFunction &parseCondition, const LeftTokenParseFunction &parseCallback, const std::vector<Token::Type> &tokenTypes)
		{
			GM_DEBUG_ASSERT(associativePrecedence != GM_FLEX_SHADER_INVALID_ASSOCIATIVE_PRECEDENCE, "Failed to add conditional left token types | Invalid associative precedence");

			for(const Token::Type i : tokenTypes)
			{
				m_LeftTokenConditionalParseConditions[(uint8_t)i].emplace_back(parseCondition);
				m_LeftTokenConditionalParseCallbacks[(uint8_t)i].emplace_back(parseCallback);
				m_LeftTokenLeftAssociativePrecedences[(uint8_t)i]  = associativePrecedence;
				m_LeftTokenRightAssociativePrecedences[(uint8_t)i] = associativePrecedence;
			}
		}

		AST::Node::Abstract *Parser::Parse(const int8_t rightAssociativePrecedence)
		{
			Token &token = m_Tokens[m_CursorPosition++];

			AST::Node::Abstract *node = nullptr; 
			for(uint8_t i = 0; i < m_NullTokenConditionalParseConditions[(uint8_t)token.type].size(); i++)
			{
				if(m_NullTokenConditionalParseConditions[(uint8_t)token.type][i](*this, token))
				{
					node = m_NullTokenConditionalParseCallbacks[(uint8_t)token.type][i](*this, m_NullTokenAssociativePrecedences[(uint8_t)token.type], token);
					break;
				}
			}

			if(!node)
				node = m_NullTokenParseCallbacks[(uint8_t)token.type](*this, m_NullTokenAssociativePrecedences[(uint8_t)token.type], token);

			while(m_CursorPosition < m_Tokens.size())
			{
				token = m_Tokens[m_CursorPosition];

				if(m_LeftTokenLeftAssociativePrecedences[(uint8_t)token.type] <= rightAssociativePrecedence)
					break;

				m_CursorPosition++;

				bool isLeftTokenConditionalParseFound = false;
				for(uint8_t i = 0; i < m_LeftTokenConditionalParseConditions[(uint8_t)token.type].size(); i++)
				{
					if(m_LeftTokenConditionalParseConditions[(uint8_t)token.type][i](*this, token, node))
					{
						node = m_LeftTokenConditionalParseCallbacks[(uint8_t)token.type][i](*this, m_LeftTokenRightAssociativePrecedences[(uint8_t)token.type], token, node);
						isLeftTokenConditionalParseFound = true;
						break;
					}
				}

				if(!isLeftTokenConditionalParseFound)
					node = m_LeftTokenParseCallbacks[(uint8_t)token.type](*this, m_LeftTokenRightAssociativePrecedences[(uint8_t)token.type], token, node);
			}

			return node;
		}
	}
}