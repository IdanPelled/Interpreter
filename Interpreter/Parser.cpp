#pragma once
#include "Parser.h"

using namespace parser;

inline token::Token Parser::next_token() {
	return tokens[index++];
}

Parser::Parser(const string& code) : tokens{ lexer::Lexer(code).lex() }, index{ 0 }
{
	tokens.push_back(token::TokenType::END_OF_TOKENS);
}

MulExp Parser::parseMulExp() {
	MulExp ret;

	ret.multiplay = next_token();
	ret.value = next_token();

	return ret;
}

NumExp Parser::parseNumExp() {
	NumExp ret;
	vector<MulExp> muls;

	ret.value = next_token();

	while (tokens[index].get_type() == token::TokenType::MULTIPLY)
		muls.push_back(parseMulExp());

	ret.multiplys = muls;
	return ret;
}

PlusExp Parser::parsePlusExp() {
	PlusExp ret;

	ret.plus = next_token();
	ret.num = parseNumExp();

	return ret;
}

ArithmeticExp Parser::parseArithmeticExp() {
	ArithmeticExp ret;
	vector<PlusExp> pluses;

	ret.num = parseNumExp();

	while (
		tokens[index].get_type() == token::TokenType::PLUS
		|| tokens[index].get_type() == token::TokenType::MINUS
	)
		pluses.push_back(parsePlusExp());

	ret.pluses = pluses;
	return ret;
}

Expression Parser::parseExpression() {
	Expression ret;
	int i = index;

	switch (tokens[index].get_type())
	{
	case token::TokenType::IDENTI:
		ret.identifier = next_token();
		ret.type = Type::IDENTIFIER;
		break;

	case token::TokenType::INT:
		ret.math_exp = parseArithmeticExp();
		ret.type = Type::INTEGER;
		break;

	case token::TokenType::STR:
		ret.string = next_token();
		ret.type = Type::STRING;
		break;

	case token::TokenType::BOOL:
		ret.boolean = parseBoolExp();
		ret.type = Type::BOOLEAN;
		break;

	default:
		throw std::invalid_argument("Syntax Error");
	}

	switch (tokens[index].get_type()) {

	case token::TokenType::PLUS:
	case token::TokenType::MINUS:
	case token::TokenType::MULTIPLY:
		index = i;
		ret.type = Type::INTEGER;
		ret.math_exp = parseArithmeticExp();
		break;

	case token::TokenType::EQUAL:
	case token::TokenType::GREATER:
	case token::TokenType::SMALLER:
		index = i;
		ret.type = Type::BOOLEAN;
		ret.boolean = parseBoolExp();
		break;

	default:
		break;
	}
	

	return ret;
}

bool is_bool_operator(token::Token op) {
	return (
		op.get_type() == token::TokenType::EQUAL
		|| op.get_type() == token::TokenType::GREATER
		|| op.get_type() == token::TokenType::SMALLER
	);
}		

Operand Parser::parseOperand() {
	Operand ret;
	switch (tokens[index].get_type())
	{
	case token::TokenType::IDENTI:
		ret.identifier = next_token();
		ret.type = Type::IDENTIFIER;
		break;

	case token::TokenType::INT:
		ret.math_exp = parseArithmeticExp();
		ret.type = Type::INTEGER;
		break;

	case token::TokenType::STR:
		ret.string = next_token();
		ret.type = Type::STRING;
		break;

	case token::TokenType::BOOL:
		ret.boolean = next_token();
		ret.type = Type::BOOLEAN;
		break;

	default:
		throw std::invalid_argument("Syntax Error");
	}

	return ret;
}

BoolExp Parser::parseBoolExp() {
	BoolExp ret;

	ret.left = parseOperand();
	
	if (!is_bool_operator(tokens[index])) {
		ret.type = BoolType::EXPRESSION;
		return ret;
	}

	ret.op = next_token();
	ret.right = parseOperand();
	ret.type = BoolType::QUERY;

	return ret;
}

AssignmentExp Parser::parseAssignmentExp() {
	AssignmentExp ret;

	ret.identifier = next_token();
	ret.assignment = next_token();
	ret.value = parseExpression();
	ret.type = ret.value.type;
	return ret;
}

Statement Parser::parseStatement() {
	Statement ret;

	switch (tokens[index].get_type())
	{
	case token::TokenType::IF:
		ret.type = StatementType::IF_TYPE;
		ret.if_statement = parseIfExp();
		break;

	case token::TokenType::IDENTI:
		ret.type = StatementType::ASSIGNMENT_TYPE;
		ret.assignment_statement = parseAssignmentExp();
		break;

	case token::TokenType::OPEN:
		ret.type = StatementType::BLOCK_TYPE;
		ret.block_statement = parseBlockExp();
		break;

	default:
		throw std::invalid_argument("Syntax Error");
		break;
	}

	return ret;
}

BlockExp Parser::parseBlockExp() {
	BlockExp ret;
	vector<Statement> statements;

	ret.open_brackets = next_token();

	while (tokens[index].get_type() != token::TokenType::CLOSE)
		statements.push_back(parseStatement());

	ret.statements = statements;
	ret.close_brackets = next_token();

	return ret;
}

IfExp Parser::parseIfExp() {
	IfExp ret;

	ret.if_token = next_token();
	ret.exp = parseBoolExp();
	ret.if_block = parseBlockExp();

	if (tokens[index].get_type() == token::TokenType::ELSE) {

		ret.else_token = next_token();
		ret.else_block = parseBlockExp();
	}

	return ret;
}

File Parser::parseFile() {
	File ret;

	while (tokens[index].get_type() != token::TokenType::END_OF_TOKENS)
		ret.code.push_back(parseStatement());

	return ret;
}
