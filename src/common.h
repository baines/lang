#ifndef EL3_COMMON_H_
#define EL3_COMMON_H_
#include "tokens.h"
#include "altlib/alt.h"
#include <functional>

namespace el3 {

	struct TokenStream;
	struct Context;
	struct Stack;

	enum ErrorCode {
		EL3_ERR_NONE = 0,
		EL3_ERR_INVALID_TOKEN,
		EL3_ERR_BRACKET_MISMATCH,
		EL3_ERR_MULTIPLE_ARG_DECL,
		EL3_ERR_NON_SYMBOL_ARG,
		EL3_ERR_ARG_MARKER_OUTSIDE_BLOCK,
	};

	struct Status {

		Status() : errcode(EL3_ERR_NONE), token(){}
		Status(ErrorCode c, Token t) : errcode(c), token(t){}

		operator bool(){ return errcode == EL3_ERR_NONE; }

		void print();

		ErrorCode errcode;
		Token token;
	};

	static const Status no_error;

	struct NativeFunc {
		alt::StrRef name;
		std::function<void(Context&)> ptr;
	};

	void stdlib_register_funcs(Context&);

	void func_call(TokenFunc, Context& ctx, std::initializer_list<Token> args);

}

#endif
