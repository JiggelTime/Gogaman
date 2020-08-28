#pragma once

#include "Token.h"
#include "AbstractSyntaxTree.h"

namespace Gogaman
{
	namespace FlexShader
	{
		class Parser
		{
		public:
			using NullTokenParseFunction = std::function<AST::Node::Abstract *(Parser &, const int8_t, const Token &)>;
			using LeftTokenParseFunction = std::function<AST::Node::Abstract *(Parser &, const int8_t, const Token &, AST::Node::Abstract *)>;

			using NullTokenParseConditionFunction = std::function<bool(Parser &, const Token &)>;
			using LeftTokenParseConditionFunction = std::function<bool(Parser &, const Token &, const AST::Node::Abstract *)>;
		public:
			Parser(std::vector<Token> &&tokens);
			~Parser() = default;

			void AddNullTokenTypes(const int8_t bindingPower, const NullTokenParseFunction &parseCallback, const std::vector<Token::Type> &tokenTypes);
			void AddLeftTokenTypes(const int8_t bindingPower, const LeftTokenParseFunction &parseCallback, const std::vector<Token::Type> &tokenTypes);
			void AddRightTokenTypes(const int8_t bindingPower, const LeftTokenParseFunction &parseCallback, const std::vector<Token::Type> &tokenTypes);

			void AddConditionalNullTokenTypes(const int8_t bindingPower, const NullTokenParseConditionFunction &parseCondition, const NullTokenParseFunction &parseCallback, const std::vector<Token::Type> &tokenTypes);
			void AddConditionalLeftTokenTypes(const int8_t bindingPower, const LeftTokenParseConditionFunction &parseCondition, const LeftTokenParseFunction &parseCallback, const std::vector<Token::Type> &tokenTypes);

			void AdvanceCursor() { m_CursorPosition++; }

			const Token &Peek() const;

			const Token &GetCurrentToken() const;

			AST::Node::Abstract *Parse(const int8_t rightBindingPower = 0);
		private:
			NullTokenParseFunction m_NullTokenParseCallbacks[GM_FLEX_SHADER_TOKEN_TYPE_COUNT];
			int8_t                 m_NullTokenBindingPowers[GM_FLEX_SHADER_TOKEN_TYPE_COUNT];

			LeftTokenParseFunction m_LeftTokenParseCallbacks[GM_FLEX_SHADER_TOKEN_TYPE_COUNT];
			int8_t                 m_LeftTokenLeftBindingPowers[GM_FLEX_SHADER_TOKEN_TYPE_COUNT];
			int8_t                 m_LeftTokenRightBindingPowers[GM_FLEX_SHADER_TOKEN_TYPE_COUNT];

			std::vector<NullTokenParseConditionFunction> m_NullTokenConditionalParseConditions[GM_FLEX_SHADER_TOKEN_TYPE_COUNT];
			std::vector<NullTokenParseFunction>          m_NullTokenConditionalParseCallbacks[GM_FLEX_SHADER_TOKEN_TYPE_COUNT];
			std::vector<int8_t>                          m_NullTokenConditionalBindingPowers[GM_FLEX_SHADER_TOKEN_TYPE_COUNT];

			std::vector<LeftTokenParseConditionFunction> m_LeftTokenConditionalParseConditions[GM_FLEX_SHADER_TOKEN_TYPE_COUNT];
			std::vector<LeftTokenParseFunction>          m_LeftTokenConditionalParseCallbacks[GM_FLEX_SHADER_TOKEN_TYPE_COUNT];

			std::vector<Token> m_Tokens;
			uint32_t           m_CursorPosition;
		};

		AST::Node::Abstract *NullParseError(Parser &parser, const int8_t bindingPower, const Token &token);
		AST::Node::Abstract *LeftParseError(Parser &parser, const int8_t rightBindingPower, const Token &token, AST::Node::Abstract *leftNode);
	}
}