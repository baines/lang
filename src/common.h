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
	void error_print(ErrorCode e, Token t);

	struct TypeMismatch {
		TokenType expected, got;
	};
	struct UndeclaredIdentifier {
		TokenIdentifier id;
	};

	struct NativeFunc {
		alt::StrRef name;
		std::function<void(Context&)> ptr;
	};

	void stdlib_register_funcs(Context&);

	void func_call(TokenFunc, Context& ctx, std::initializer_list<Token> args);
}

#endif
