#pragma once
#include <string>
#include <map>

using std::string;
using std::map;

namespace token
{
	typedef enum class TokenType {
		// KeyWords:
		OPEN,
		CLOSE,
		IF,
		ELSE,
		TRUE,
		FALSE,

		// Identifiers:
		IDENTI,

		// Literals:
		STR,
		INT,
		BOOL,

		// Operators
		ASSIGNMENT,
		PLUS,
		MINUS,
		MULTIPLY,
		EQUAL,
		GREATER,
		SMALLER,

		// No a token
		END_OF_TOKENS,

	} token_t;

	class Token {
	public:
		Token(token_t type);
		Token();

		token_t get_type();
		string get_data(string key);
		bool operator == (Token& t);
		bool operator < (Token& t);
		bool operator > (Token& t);

	protected:
		token_t type;
		map<string, string> data;
	};

	class KeyWord : public Token {
	public:
		KeyWord(token_t type);
	};

	class Identifier : public Token {
	public:
		Identifier(string& name);
	};

	class Literal : public Token {
	public:
		Literal(string& value);
	};

	class Operator : public Token {
	public:
		Operator(token_t type);
	};
}