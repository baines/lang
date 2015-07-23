#include "el3.h"
#include <vector>

using namespace el3;

inline void skip_whitespace(const char*& code){
	while(*code <= ' ') code++;
}

inline void lex_number(const char*& code, std::vector<Token>& tokens){
	//TODO: floating point
	int result = 0;
	
	do {
		result = (result * 10) + (*code - '0');
	} while(++code, *code >= '0' && *code <= '9');
	
	tokens.push_back(Token(TokenType::number, result));
}

inline void lex_identifier(const char*& code, std::vector<Token>& tokens){
	const char* name = code;

	while(isalnum(*code++));
	
	tokens.emplace_back(Token(TokenType::id, name, code - name));
}

inline void lex_string(const char*& code, std::vector<Token>& tokens){
	const char* name = ++code;

	//TODO: handle escape characters
	while(*code && *code++ != '\'');

	tokens.emplace_back(Token(TokenType::string, name, code - name));
}

void Context::lex(const char* code, std::vector<Token>& tokens){
	while(*code){
		skip_whitespace(code);

		TokenType tt = TokenType::invalid;
				
		switch(*code){
			case '{': tt = TokenType::block_start; break;
			case '}': tt = TokenType::block_end;   break;
			case '(': tt = TokenType::func_start;  break;
			case ')': tt = TokenType::func_end;    break;
			case '[': tt = TokenType::list_start;  break;
			case ']': tt = TokenType::list_end;    break;
		}
		
		if(tt != TokenType::invalid){
			tokens.emplace_back(tt);
			++code;
		} else if(*code >= '0' && *code <= '9'){
			lex_number(code, tokens);
		} else if(*code == '\''){
			lex_string(code, tokens);
		} else {
			lex_identifier(code, tokens);
		}
	}
}

