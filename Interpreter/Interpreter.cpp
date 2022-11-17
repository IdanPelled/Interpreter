#include "Interpreter.h"
#include <iostream>
#include <fstream>
#include <string>

#define PATH "Code.txt"

using namespace interpreter;

const string read_file(const string& path) {
	std::ifstream file;
	std::stringstream buffer;

	file.open(path);
	buffer << file.rdbuf();
	return buffer.str();
}

Interpreter::Interpreter(const string& path)
	: code { parser::Parser(read_file(path)).parseFile() }
{
}

Variable Interpreter::get_var(string name) {
	return vars[name];
}

int Interpreter::interprerMulExp(parser::MulExp exp) {
	if (exp.multiplay.get_type() != token::TokenType::MULTIPLY)
		throw std::invalid_argument("Expecting for token +/-");

	return stoi(exp.value.get_data("value"));
}

int Interpreter::interprerPlusExp(parser::PlusExp exp) {
	token::TokenType type = exp.plus.get_type();
	
	if (type == token::TokenType::MINUS || type == token::TokenType::PLUS)
		return interprerNumExp(exp.num);

	throw std::invalid_argument("Expecting for token +/-");

}

int Interpreter::interprerNumExp(parser::NumExp exp) {
	int ret;

	if (exp.value.get_type() == token::TokenType::IDENTI) {
		Variable var = get_var(exp.value.get_data("name"));
		if (var.type != VarType::Integer)
			throw std::invalid_argument("Variable isnt of type integer");
		ret = var.int_val;
	}
	else
		ret = stoi(exp.value.get_data("value"));
	
	for (parser::MulExp mul : exp.multiplys)
		ret *= interprerMulExp(mul);

	return ret;
}

int Interpreter::interprerArithmeticExp(parser::ArithmeticExp exp)
{
	int tmp;
	int ret = interprerNumExp(exp.num);

	for (parser::PlusExp plus : exp.pluses) {
		tmp = interprerPlusExp(plus);
		
		if (plus.plus.get_type() == token::TokenType::PLUS)
			ret += tmp;

		if (plus.plus.get_type() == token::TokenType::MINUS)
			ret -= tmp;
	}

	return ret;
}

ReturnType Interpreter::interprerExpression(parser::Operand exp) {
	ReturnType ret;
	Variable var;

	switch (exp.type)
	{
	case parser::Type::IDENTIFIER:
		var = get_var(exp.identifier.get_data("name"));

		switch (var.type)
		{
		case VarType::Integer:
			ret.integer = var.int_val;
			break;

		case VarType::Boolean:
			ret.boolean = var.bool_val;
			break;

		case VarType::String:
			ret.string = var.string_val;
			break;
		}
		ret.type = var.type;
		break;
	
	case parser::Type::BOOLEAN:
		ret.boolean = evaluate_bool(exp.boolean.get_data("value"));
		ret.type = VarType::Boolean;
		break;

	case parser::Type::STRING:
		ret.string = exp.string.get_data("value");
		ret.type = VarType::String;
		break;

	case parser::Type::INTEGER:
		ret.integer = interprerArithmeticExp(exp.math_exp);
		ret.type = VarType::Integer;
		break;
	}

	return ret;
}

bool Interpreter::evaluate_bool(string s) {
	if (s.compare("True") == 0)
		return true;

	if (s.compare("False") == 0)
		return false;

	throw std::invalid_argument("invalid bool argument");
}


bool evaluate_equal(ReturnType left, ReturnType right) {
	if (left.type != right.type)
		throw std::invalid_argument("Cant compare diffrent types");

	switch (left.type)
	{
	case VarType::Boolean:
		return left.boolean && right.boolean;

	case VarType::String:
		return left.string.compare(right.string) == 0;

	case VarType::Integer:
		return left.integer == right.integer;
	
	default:
		throw std::invalid_argument("Invalid type");
	}
}

bool evaluate_grater(ReturnType left, ReturnType right) {
	if (left.type != right.type)
		throw std::invalid_argument("Cant compare diffrent types");

	switch (left.type)
	{
	case VarType::Boolean:
		throw std::invalid_argument("Cant use operator < on type boolean");

	case VarType::String:
		throw std::invalid_argument("Cant use operator < on type string");

	case VarType::Integer:
		return left.integer > right.integer;

	default:
		throw std::invalid_argument("Invalid type");
	}
}

bool evaluate_smaller(ReturnType left, ReturnType right) {
	if (left.type != right.type)
		throw std::invalid_argument("Cant compare diffrent types");

	switch (left.type)
	{
	case VarType::Boolean:
		throw std::invalid_argument("Cant use operator > on type boolean");

	case VarType::String:
		throw std::invalid_argument("Cant use operator > on type string");

	case VarType::Integer:
		return left.integer < right.integer;

	default:
		throw std::invalid_argument("Invalid type");
	}
}

bool Interpreter::interprerBoolExp(parser::BoolExp exp) {
	if (exp.type == parser::BoolType::EXPRESSION)
		if (exp.left.type == parser::Type::BOOLEAN)
			return interprerExpression(exp.left).boolean;
		else
			if (exp.left.type == parser::Type::IDENTIFIER
				&& get_var(exp.left.identifier.get_data("name")).type == VarType::Boolean)
				return get_var(exp.left.identifier.get_data("name")).bool_val;
			else
				throw std::invalid_argument("Not a boolean expression");

	else if (exp.type == parser::BoolType::QUERY) {
		ReturnType left = interprerExpression(exp.left);
		ReturnType right = interprerExpression(exp.right);

		switch (exp.op.get_type())
		{
		case token::TokenType::EQUAL:
			return evaluate_equal(left, right);
		case token::TokenType::GREATER:
			return evaluate_grater(left, right);
		case token::TokenType::SMALLER:
			return evaluate_smaller(left, right);
		default:
			break;
		}
	}
	throw std::invalid_argument("Not a boolean expression");
}

void Interpreter::interprerAssignment(parser::AssignmentExp exp) {
	Variable var;

	if (exp.assignment.get_type() != token::TokenType::ASSIGNMENT)
		throw std::invalid_argument("Syntax error");

	var.name = exp.identifier.get_data("name");
	switch (exp.type)
	{
	case parser::Type::INTEGER:
		var.int_val = interprerArithmeticExp(exp.value.math_exp);
		var.type = VarType::Integer;
		break;

	case parser::Type::STRING:
		var.string_val = exp.value.string.get_data("value");
		var.type = VarType::String;
		break;

	case parser::Type::BOOLEAN:
		var.bool_val = interprerBoolExp(exp.value.boolean);
		var.type = VarType::Boolean;
		break;

	case parser::Type::IDENTIFIER:
		switch (get_var(exp.value.identifier.get_data("name")).type)
		{
		case VarType::Integer:
			var.int_val = interprerArithmeticExp(exp.value.math_exp);
			var.type = VarType::Integer;
			break;
		case VarType::String:
			var.string_val = exp.value.string.get_data("value");
			var.type = VarType::String;
			break;
		case VarType::Boolean:
			var.bool_val = interprerBoolExp(exp.value.boolean);
			var.type = VarType::Boolean;
			break;
		default:
			break;
		}
		break;

	default:
		throw std::invalid_argument("Syntax error");
	}

	set_vat(var);
}

void Interpreter::interprerBlock(parser::BlockExp exp) {
	if (exp.open_brackets.get_type() != token::TokenType::OPEN)
		throw std::invalid_argument("Syntax error");

	for (parser::Statement statement : exp.statements)
		interprerStatement(statement);

	if (exp.close_brackets.get_type() != token::TokenType::CLOSE)
		throw std::invalid_argument("Syntax error");
}

void Interpreter::interprerIf(parser::IfExp exp) {
	if (exp.if_token.get_type() != token::TokenType::IF)
		throw std::invalid_argument("Syntax error");

	if (interprerBoolExp(exp.exp))
		interprerBlock(exp.if_block);
	
	else if (exp.else_token.get_type() == token::TokenType::ELSE)
		interprerBlock(exp.else_block);
}

void Interpreter::interprerStatement(parser::Statement exp) {
	switch (exp.type)
	{
	case parser::StatementType::ASSIGNMENT_TYPE:
		interprerAssignment(exp.assignment_statement);
		break;

	case parser::StatementType::IF_TYPE:
		interprerIf(exp.if_statement);
		break;

	case parser::StatementType::BLOCK_TYPE:
		interprerBlock(exp.block_statement);
		break;

	default:
		throw std::invalid_argument("Syntax error");
	}
}

void Interpreter::interprerFile(parser::File exp) {
	for (parser::Statement statement : exp.code)
		interprerStatement(statement);
}

void Interpreter::set_vat(Variable& var)
{
	vars[var.name] = var;
}

bool Interpreter::is_var(string& name)
{
	return vars.find(name) != vars.end();
}

void Interpreter::interprer()
{
	interprerFile(code);
	std::cout << get_var("message").string_val << std::endl;
}



int main() {
	Interpreter(PATH).interprer();
}