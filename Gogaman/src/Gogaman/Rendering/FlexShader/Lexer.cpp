#include "pch.h"
#include "Lexer.h"

//#include "Gogaman/Core/Base.h"

#include "Token.h"

namespace Gogaman
{
	namespace FlexShader
	{
		std::vector<Token> Tokenize(const std::string &source)
		{
			std::vector<Token> tokens;
	
			uint32_t cursorPosition = 0;
			while(cursorPosition < source.length())
			{
				const char character = source[cursorPosition];

				auto AddToken = [&](const Token::Type type, std::string_view &&lexeme)
				{
					Token &token = tokens.emplace_back();
					token.type   = type;
					token.lexeme = lexeme;
				};
	
				auto AddAtom = [&](const Token::Type type)
				{
					AddToken(type, std::string_view(source.c_str() + cursorPosition, 1));
				};
	
				if(isspace(character))
				{
					cursorPosition++;
					continue;
				}
	
				if(isdigit(character))
				{
					const uint32_t initialPosition = cursorPosition;
	
					while(true)
					{
						cursorPosition++;
	
						if(!isdigit(source[cursorPosition]))
							break;
					}
	
					AddToken(Token::Type::Number, std::string_view(source.c_str() + initialPosition, cursorPosition - initialPosition));
					continue;
				}
	
				if(isalpha(character) || character == '_')
				{
					const uint32_t initialPosition = cursorPosition;
	
					while(true)
					{
						cursorPosition++;
	
						if(!isalnum(source[cursorPosition]) && source[cursorPosition] != '_')
							break;
					}

					AddToken(Token::Type::Identifier, std::string_view(source.c_str() + initialPosition, cursorPosition - initialPosition));
					continue;
				}

				if(character == '"')
				{
					const uint32_t initialPosition = cursorPosition;

					while(true)
					{
						cursorPosition++;

						//GM_DEBUG_ASSERT(cursorPosition < source.length(), "Failed to tokenize FlexShader");

						if(source[cursorPosition] == '"')
							break;
					}

					AddToken(Token::Type::String, std::string_view(source.c_str() + initialPosition + 1, cursorPosition - (initialPosition + 1)));
				}
	
				switch(character)
				{
				case ':':
					AddAtom(Token::Type::Colon);
					break;
				case ';':
					AddAtom(Token::Type::Semicolon);
					break;
				case '(':
					AddAtom(Token::Type::LeftParenthesis);
					break;
				case ')':
					AddAtom(Token::Type::RightParenthesis);
					break;
				case '[':
					AddAtom(Token::Type::LeftSquareBracket);
					break;
				case ']':
					AddAtom(Token::Type::RightSquareBracket);
					break;
				case '{':
					AddAtom(Token::Type::LeftBrace);
					break;
				case '}':
					AddAtom(Token::Type::RightBrace);
					break;
				case '=':
					AddAtom(Token::Type::Equal);
					break;
				case '<':
					AddAtom(Token::Type::Less);
					break;
				case '>':
					AddAtom(Token::Type::Greater);
					break;
				case '+':
					AddAtom(Token::Type::Plus);
					break;
				case '-':
					AddAtom(Token::Type::Minus);
					break;
				default:
					break;
				}
	
				cursorPosition++;
			}
	
			return tokens;
		}
	}
}