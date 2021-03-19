#include "pch.h"
#include "Parser.h"

#include "Gogaman/Core/Base.h"

#define GM_FLEX_SHADER_INVALID_BINDING_POWER INT8_MAX

namespace Gogaman
{
	namespace FlexShader
	{
		using NullTokenParseFunction = std::function<AST::Node::Abstract *(Parser &, const int8_t, const Token &)>;
		using LeftTokenParseFunction = std::function<AST::Node::Abstract *(Parser &, const int8_t, const Token &, AST::Node::Abstract *)>;

		AST::Node::Abstract *NullParseError(Parser &parser, const int8_t bindingPower, const Token &token)
		{
			GM_DEBUG_ASSERT(false, "Failed to parse FlexShader | Invalid syntax | Line: %d | Token \"%s\" is invalid", token.line, GetTokenString(token).c_str());
		}

		AST::Node::Abstract *LeftParseError(Parser &parser, const int8_t rightBindingPower, const Token &token, AST::Node::Abstract *leftNode)
		{
			GM_DEBUG_ASSERT(false, "Failed to parse FlexShader | Invalid syntax | Line: %d | Token \"%s\" is invalid", token.line, GetTokenString(token).c_str());
		}

		const Token &Parser::Peek() const
		{
			GM_DEBUG_ASSERT(m_CursorPosition + 1 < m_Tokens.size(), "Failed to peek | End of tokens reached");

			return m_Tokens[m_CursorPosition + 1];
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
				//m_NullTokenBindingPowers[i]     = GM_FLEX_SHADER_INVALID_BINDING_POWER;
				//m_LeftTokenLeftBindingPowers[i] = GM_FLEX_SHADER_INVALID_BINDING_POWER;
				m_NullTokenBindingPowers[i]     = 0;
				m_LeftTokenLeftBindingPowers[i] = 0;

				m_NullTokenParseCallbacks[i] = NullParseError;
				m_LeftTokenParseCallbacks[i] = LeftParseError;
			}
		}

		void Parser::AddNullTokenTypes(const int8_t bindingPower, const NullTokenParseFunction &parseCallback, const std::vector<Token::Type> &tokenTypes)
		{
			GM_DEBUG_ASSERT(bindingPower != GM_FLEX_SHADER_INVALID_BINDING_POWER, "Failed to add null token types | Invalid binding power");

			for(const Token::Type i : tokenTypes)
			{
				m_NullTokenParseCallbacks[(uint8_t)i] = parseCallback;
				m_NullTokenBindingPowers[(uint8_t)i] = bindingPower;

				if(m_LeftTokenLeftBindingPowers[(uint8_t)i] == GM_FLEX_SHADER_INVALID_BINDING_POWER)
				{
					m_LeftTokenParseCallbacks[(uint8_t)i]     = LeftParseError;
					m_LeftTokenLeftBindingPowers[(uint8_t)i]  = 0;
					m_LeftTokenRightBindingPowers[(uint8_t)i] = 0;
				}
			}
		}

		void Parser::AddLeftTokenTypes(const int8_t bindingPower, const LeftTokenParseFunction &parseCallback, const std::vector<Token::Type> &tokenTypes)
		{
			GM_DEBUG_ASSERT(bindingPower != GM_FLEX_SHADER_INVALID_BINDING_POWER, "Failed to add left token types | Invalid binding power");

			for(const Token::Type i : tokenTypes)
			{
				m_LeftTokenParseCallbacks[(uint8_t)i]     = parseCallback;
				m_LeftTokenLeftBindingPowers[(uint8_t)i]  = bindingPower;
				m_LeftTokenRightBindingPowers[(uint8_t)i] = bindingPower;

				if(m_NullTokenBindingPowers[(uint8_t)i] == GM_FLEX_SHADER_INVALID_BINDING_POWER)
				{
					m_NullTokenParseCallbacks[(uint8_t)i] = NullParseError;
					m_NullTokenBindingPowers[(uint8_t)i]  = 0;
				}
			}
		}

		void Parser::AddRightTokenTypes(const int8_t bindingPower, const LeftTokenParseFunction &parseCallback, const std::vector<Token::Type> &tokenTypes)
		{
			GM_DEBUG_ASSERT(bindingPower != GM_FLEX_SHADER_INVALID_BINDING_POWER, "Failed to add right token types | Invalid binding power");

			for(const Token::Type i : tokenTypes)
			{
				m_LeftTokenParseCallbacks[(uint8_t)i]              = parseCallback;
				m_LeftTokenLeftBindingPowers[(uint8_t)i]  = bindingPower;
				m_LeftTokenRightBindingPowers[(uint8_t)i] = bindingPower - 1;

				if(m_NullTokenBindingPowers[(uint8_t)i] == GM_FLEX_SHADER_INVALID_BINDING_POWER)
				{
					m_NullTokenParseCallbacks[(uint8_t)i]         = NullParseError;
					m_NullTokenBindingPowers[(uint8_t)i] = 0;
				}
			}
		}

		void Parser::AddConditionalNullTokenTypes(const int8_t bindingPower, const NullTokenParseConditionFunction &parseCondition, const NullTokenParseFunction &parseCallback, const std::vector<Token::Type> &tokenTypes)
		{
			GM_DEBUG_ASSERT(bindingPower != GM_FLEX_SHADER_INVALID_BINDING_POWER, "Failed to add conditional null token types | Invalid binding power");

			for(const Token::Type i : tokenTypes)
			{
				m_NullTokenConditionalParseConditions[(uint8_t)i].emplace_back(parseCondition);
				m_NullTokenConditionalParseCallbacks[(uint8_t)i].emplace_back(parseCallback);
				//m_NullTokenBindingPowers[(uint8_t)i] = bindingPower;
				m_NullTokenConditionalBindingPowers[(uint8_t)i].emplace_back(bindingPower);

				//figure this out. new
				if(m_LeftTokenLeftBindingPowers[(uint8_t)i] == GM_FLEX_SHADER_INVALID_BINDING_POWER)
				{
					m_LeftTokenParseCallbacks[(uint8_t)i] = LeftParseError;
					m_LeftTokenLeftBindingPowers[(uint8_t)i] = 0;
					m_LeftTokenRightBindingPowers[(uint8_t)i] = 0;
				}
			}
		}

		void Parser::AddConditionalLeftTokenTypes(const int8_t bindingPower, const LeftTokenParseConditionFunction &parseCondition, const LeftTokenParseFunction &parseCallback, const std::vector<Token::Type> &tokenTypes)
		{
			GM_DEBUG_ASSERT(bindingPower != GM_FLEX_SHADER_INVALID_BINDING_POWER, "Failed to add conditional left token types | Invalid binding power");

			for(const Token::Type i : tokenTypes)
			{
				m_LeftTokenConditionalParseConditions[(uint8_t)i].emplace_back(parseCondition);
				m_LeftTokenConditionalParseCallbacks[(uint8_t)i].emplace_back(parseCallback);
				m_LeftTokenLeftBindingPowers[(uint8_t)i]  = bindingPower;
				m_LeftTokenRightBindingPowers[(uint8_t)i] = bindingPower;
			}
		}

		AST::Node::Abstract *Parser::Parse(const int8_t rightBindingPower)
		{
			Token &token = m_Tokens[m_CursorPosition++];

			AST::Node::Abstract *node = nullptr; 
			for(uint8_t i = 0; i < m_NullTokenConditionalParseConditions[(uint8_t)token.type].size(); i++)
			{
				if(m_NullTokenConditionalParseConditions[(uint8_t)token.type][i](*this, token))
				{
					node = m_NullTokenConditionalParseCallbacks[(uint8_t)token.type][i](*this, m_NullTokenConditionalBindingPowers[(uint8_t)token.type][i], token);
					break;
				}
			}

			if(!node)
				node = m_NullTokenParseCallbacks[(uint8_t)token.type](*this, m_NullTokenBindingPowers[(uint8_t)token.type], token);

			while(m_CursorPosition < m_Tokens.size())
			{
				token = m_Tokens[m_CursorPosition];

				if(m_LeftTokenLeftBindingPowers[(uint8_t)token.type] <= rightBindingPower)
					break;

				m_CursorPosition++;

				bool isLeftTokenConditionalParseFound = false;
				for(uint8_t i = 0; i < m_LeftTokenConditionalParseConditions[(uint8_t)token.type].size(); i++)
				{
					if(m_LeftTokenConditionalParseConditions[(uint8_t)token.type][i](*this, token, node))
					{
						node = m_LeftTokenConditionalParseCallbacks[(uint8_t)token.type][i](*this, m_LeftTokenRightBindingPowers[(uint8_t)token.type], token, node);
						isLeftTokenConditionalParseFound = true;
						break;
					}
				}

				if(!isLeftTokenConditionalParseFound)
					node = m_LeftTokenParseCallbacks[(uint8_t)token.type](*this, m_LeftTokenRightBindingPowers[(uint8_t)token.type], token, node);
			}

			return node;
		}
	}
}