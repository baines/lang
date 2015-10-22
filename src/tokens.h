#ifndef EL3_TOKENS_H_
#define EL3_TOKENS_H_
#include <cstdint>
#include <cstddef>
#include <cstdio>

namespace el3 {

struct NativeFunc;

enum TokenType : uint_fast8_t {
/* lexable tokens */
	TKN_INVALID,
	TKN_ID,
	TKN_NUMBER,
	TKN_STRING,
	TKN_SYMBOL,
	TKN_ARGS_MARKER,
	TKN_FUNC_START,
	TKN_FUNC_END,
	TKN_LIST_START,
	TKN_LIST_END,
	TKN_BLOCK_START,
	TKN_BLOCK_END,

/* not lexable, only appear on stack. */
	TKN_FUNC,
	TKN_LIST,
	TKN_STACK_FRAME,

	NUM_TOKENS
};

struct TokenNumber { int num; };
struct TokenString { const char* str; uint32_t len; };
struct TokenSymbol { const char* str; uint32_t len; };
struct TokenIdentifier { const char* str; uint32_t len; };
struct TokenStackFrame { size_t num; };

enum FuncType : uint_fast8_t { FN_NIL, FN_NATIVE, FN_BLOCK };
enum ListType : uint_fast8_t { LIST_NIL, LIST_NATIVE, LIST_STACK };

struct TokenFunc {
	FuncType type;

	uint32_t block_start, block_end;
	NativeFunc* native;

};

struct TokenList {
	ListType type;	
	uint32_t stack_start, stack_end;
	//NativeList* start_native;
};

struct Token {
	TokenType type;

	union {
		TokenIdentifier id;
		TokenString     str;
		TokenSymbol     sym;
		TokenNumber     num;
		TokenFunc       func;
		TokenList       list;
		TokenStackFrame frame;
	};

	uint32_t source_index;

	Token() = default;
	Token(TokenType t) : type(t){}
	Token(const TokenIdentifier& t) : type(TKN_ID), id(t){}
	Token(const TokenString& t) : type(TKN_STRING), str(t){}
	Token(const TokenSymbol& t) : type(TKN_SYMBOL), sym(t){}
	Token(const TokenNumber& t) : type(TKN_NUMBER), num(t){}
	Token(const TokenFunc& t) : type(TKN_FUNC), func(t){}
	Token(const TokenList& t) : type(TKN_LIST), list(t){}
	Token(const TokenStackFrame& t) : type(TKN_STACK_FRAME), frame(t){}
	
	bool operator==(TokenType t) const { return type == t; }
	operator bool(){ return type != TKN_INVALID; }
};

static const char* token_names[][2] = {
	{ "INV", "INVALID" },
	{ "ID ", "ID" },
	{ "NUM", "NUMBER" },
	{ "STR", "STRING" },
	{ "SYM", "SYMBOL" },
	{ "->" , "ARGS_MARKER" },
	{ "("  , "FUNC_START" },
	{ ")"  , "FUNC_END" },
	{ "["  , "LIST_START" },
	{ "]"  , "LIST_END" },
	{ "{"  , "BLOCK_START" },
	{ "}"  , "BLOCK_END"},
	{ "FN" , "FUNC" },
	{ "LST", "LIST" },
	{ "|"  , "STACK BOUNDARY" },
	{ "???", "UNKNOWN" },
};

inline const char* token_name(Token t){ return token_names[t.type][0]; }
inline const char* token_name_full(Token t){ return token_names[t.type][1]; }

void token_print(Token t);

}

#endif
