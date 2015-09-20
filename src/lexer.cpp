#include "el3.h"
#include <vector>

using namespace el3;

inline Token lex_number(const char*& code){
	//TODO: floating point
	int result = 0;
	
	for(; *code; ++code){
		result = (result * 10) + (*code - '0');
		if(*(code + 1) <= '0' || *(code + 1) >= '9') break;
	}
	
	return Token(TokenType::number, result);
}

constexpr alt::StrRef reserved_chars("()[]{}:\'");

inline Token lex_identifier(const char*& code){
	const char* name = code;

	char c;
	while(c = *(code+1), reserved_chars.find(c) == reserved_chars.end() && isgraph(c)){
		++code;
	}
	
	return Token(TokenType::id, name, (code - name) + 1);
}

inline Token lex_dash(const char*& code){
	char next = *(code + 1);

	if(next == '>'){
		++code;
		return TokenType::args_marker;
	} else if(next >= '0' && next <= '9'){
		Token t = lex_number(++code);
		t.data = uintptr_t(t.get<int>() * -1);
		return t;
	} else {
		return lex_identifier(code);
	}
}

inline Token lex_symbol(const char*& code){
	Token t = lex_identifier(++code);
	if(t){
		t.type = TokenType::symbol;
	}
	return t;
}

inline Token lex_string(const char*& code){
	const char* name = ++code;

	/*TODO: Handle escape character sequences. 
	 *  I suppose the string should be modified in-place to remove the '\' by copying
	 *  characters backwards, and filling after the end of the string with spaces.
	 *  This will require making a copy of the code before lexing. */
	for(; isprint(*code); ++code){
		if(*code == '\'') break;
	}

	return Token(TokenType::string, name, code - name);
}

Status Context::lex(const char* code, std::vector<Token>& tokens){
	const char* line_start = code;
	size_t line_num = 0;

	for(; *code; ++code){
		if(*code == '\n'){
			++line_num;
			line_start = code + 1;
			continue;
		}

		if(isspace(*code)){
			continue;
		}

		Token t(TokenType::invalid);
				
		if(*code >= '0' && *code <= '9'){
			t = lex_number(code);
		} else switch(*code){
			case '{':  t = TokenType::block_start; break;
			case '}':  t = TokenType::block_end;   break;
			case '(':  t = TokenType::func_start;  break;
			case ')':  t = TokenType::func_end;    break;
			case '[':  t = TokenType::list_start;  break;
			case ']':  t = TokenType::list_end;    break;
			case '-':  t = lex_dash(code);         break;
			case ':':  t = lex_symbol(code);       break;
			case '\'': t = lex_string(code);       break;
			default: if(isgraph(*code))
					   t = lex_identifier(code);   break;
		}
		
		if(t){
			tokens.push_back(t);
		} else {
			fprintf(
				stderr,
				"script:%zu:%zu: Invalid token [%c].\n",
				line_num,
				code - line_start,
				*code
			);

			return Status(EL3_ERR_INVALID_TOKEN, t);
		}
	}

	return no_error;
}

