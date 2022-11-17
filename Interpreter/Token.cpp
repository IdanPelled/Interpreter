#include <string>
#include <stdexcept>

#include "Token.h"

using std::string;
using namespace token;

token_t Token::get_type()
{
	return this->type;
}

string Token::get_data(string key)
{
	return data[key];
}

bool Token::operator == (Token& t) {
	return get_data("value") == t.get_data("value");
}

bool Token::operator < (Token& t) {
	return get_data("value") < t.get_data("value");
}

bool Token::operator > (Token& t) {
	return get_data("value") > t.get_data("value");
}

Token::Token(token_t type) : type{ type }
{
}

Token::Token()
{
}

KeyWord::KeyWord(token_t type) : Token(type)
{
}

token_t get_literal_type(string& value) {
	if (value == "True" || value == "False")
		return TokenType::BOOL;

	if (value[0] == '\"')
		return TokenType::STR;

	if (isdigit(value[0]) || value[0] == '-')
		return TokenType::INT;

	throw std::invalid_argument("Undifined literal: " + value);
}

Literal::Literal(string& value) : Token(get_literal_type(value))
{
	data["value"] = value;
}

Identifier::Identifier(string& name) : Token(TokenType::IDENTI)
{
	data["name"] = name;
}

Operator::Operator(token_t type) : Token(type)
{
}

