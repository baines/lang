#ifndef EL3_TOKENS_H_
#define EL3_TOKENS_H_
#include <cstdint>
#include <cstddef>
#include <cstdio>

namespace el3 {

enum class TokenType : uint_fast8_t {
	invalid,
	id,
	number,
	string,
	func_start,
	func_end,
	list_start,
	list_end,
	block_start,
	block_end,
	native_func,

	num_tokens
};

struct Token {
	Token(TokenType t, const void* d, size_t s = 0)
	   	: type(t), data(reinterpret_cast<intptr_t>(d)), size(s){}
	Token(TokenType t, const intptr_t d, size_t s = 0) : type(t), data(d), size(s){}
	Token(TokenType t) : Token(t, nullptr, 0){}
	Token() : Token(TokenType::invalid, nullptr, 0){}

	operator bool() const {
		return type != TokenType::invalid;
	}

	TokenType type;
	intptr_t data;
	size_t size;
};

inline void token_print(Token& t){
	switch(t.type){
		case TokenType::func_start:
			puts("FUNC START"); break;
		case TokenType::func_end:
			puts("FUNC END"); break;
		case TokenType::list_start:
			puts("LIST START"); break;
		case TokenType::list_end:
			puts("LIST END"); break;
		case TokenType::block_start:
			puts("BLOCK START"); break;
		case TokenType::block_end:
			puts("BLOCK END"); break;
		case TokenType::id: {
			printf("ID: '%.*s'\n", (int)t.size, (const char*)t.data);
			break;
		}
		case TokenType::number: {
			printf("NUM: '%d'\n", (int)t.data);
			break;
		}
		case TokenType::string: {
			printf("STR: '%.*s'\n", (int)t.size, (const char*)t.data);
			break;
		}
		case TokenType::invalid: 
		default:
			puts("INVALID"); break;
	}
}

}

#endif
