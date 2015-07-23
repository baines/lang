#ifndef EL3_TOKENS_H_
#define EL3_TOKENS_H_
#include <cstdint>
#include <cstddef>
#include <cstdio>

namespace el3 {

enum class TokenType : uint_fast8_t {

	/* lexable tokens */

	invalid,      // < unused            , unused               >
	id,           // < const char* : name, size_t : name_length >
	number,       // < int         : val , unused               >
	string,       // < const char* : name, size_t : name_length >
	func_start,   // < unused            , unused               >
	func_end,     // < unused            , unused               >
	list_start,   // TODO 
	list_end,     // TODO
	block_start,  // < unused            , unused               >
	block_end,    // < unused            , unused               >

	/* not lexable, only appear on stack. */

	native_func,  // < Symbol* : sym     , unused               >
	block_marker, // < int     : index   , unused               >

	num_tokens
};

struct Token {
	Token(TokenType t, const void* d, size_t s = 0)
	   	: type(t), data(reinterpret_cast<intptr_t>(d)), size(s){}

	Token(TokenType t, const uintptr_t d, size_t s = 0) 
		: type(t), data(d), size(s){}

	Token(TokenType t) 
		: Token(t, nullptr, 0){}

	Token() 
		: Token(TokenType::invalid, nullptr, 0){}

	operator bool() const {
		return type != TokenType::invalid;
	}

	template<class T>
	T get() {
		return reinterpret_cast<T>(data);
	}

	const char* debug_name_short() const {
		switch(type){
			case TokenType::id:           return "id ";
			case TokenType::number:       return "num";
			case TokenType::string:       return "str";
			case TokenType::func_start:   return " ( ";
			case TokenType::func_end:     return " ) ";
			case TokenType::list_start:   return " [ ";
			case TokenType::list_end:     return " ] ";
			case TokenType::block_start:  return " { ";
			case TokenType::block_end:    return " } ";
			case TokenType::native_func:  return "fn ";
			case TokenType::block_marker: return "bm ";
			case TokenType::invalid:      return "inv";
			default:                      return "???";
		}
	}

	const char* debug_name() const {
		switch(type){
			case TokenType::id:           return "id";
			case TokenType::number:       return "number";
			case TokenType::string:       return "string";
			case TokenType::func_start:   return "func_start";
			case TokenType::func_end:     return "func_end";
			case TokenType::list_start:   return "list_start";
			case TokenType::list_end:     return "list_end";
			case TokenType::block_start:  return "block_start";
			case TokenType::block_end:    return "block_end";
			case TokenType::native_func:  return "native_func";
			case TokenType::block_marker: return "block_marker";
			case TokenType::invalid:      return "invalid";
			default:                      return "unknown";
		}
	}

	void debug_print() const {
		switch(type){
			case TokenType::id: {
				printf("id: '%.*s'\n", (int)size, (const char*)data);
				break;
			}
			case TokenType::number: {
				printf("num: '%d'\n", (int)data);
				break;
			}
			case TokenType::string: {
				printf("str: '%.*s'\n", (int)size, (const char*)data);
				break;
			}
			default:
				puts(debug_name());
				break;
		}
	}

	TokenType type;
	uintptr_t data;
	size_t size;
};

}

#endif
