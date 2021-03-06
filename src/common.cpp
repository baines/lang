#include "el3.h"

using namespace el3;

namespace el3 {

//XXX: i18n?

static const char* err_strings[] = {
	"No error.",
	"Invalid token.",
	"Bracket does not match.",
	"'->' token can only appear once per block.",
	"Only symbols can appear before '->' token.",
	"'->' token cannot appear outside a block."
};

void error_print(ErrorCode e, Token t){
	fprintf(stderr, "<stdin>:%u:%u: %s\n", t.source_line, t.source_col, err_strings[e]);
}

void token_print(Token t, FILE* f){
	fprintf(f, "[%s", token_name_full(t));

	if(t.type == TKN_NUMBER){
		fprintf(f, ": %d", t.num.num);		
	}

	if(t.type == TKN_STRING || t.type == TKN_ID || t.type == TKN_SYMBOL){
		fprintf(f, ": %.*s", t.str.len, t.str.str);
	}

	if(t.type == TKN_FUNC){
		if(t.func.type == FN_BLOCK){
			fprintf(f, ": (BLOCK @ [%d:%d])", t.func.block_start, t.func.block_end);
		} else
		if(t.func.type == FN_NATIVE){
			fprintf(
				f,
				": (NATIVE [%.*s])",
				t.func.native->name.sizei(),
				t.func.native->name.data()
			);
		} else {
			fprintf(f, ": (NIL)");
		}
	}

	if(t.type == TKN_LIST){
		if(t.list.type == LIST_STACK){
			fprintf(f, ": (STACK)");
		} else
		if(t.list.type == LIST_NATIVE){
			fprintf(f, ": (NATIVE)");
		} else {
			fprintf(f, ": (NIL)");
		}
	}

	if(t.type == TKN_STACK_FRAME){
		fprintf(f, ": (%zu)", t.frame.num);
	}
	fprintf(f, "]\n");
}

}



