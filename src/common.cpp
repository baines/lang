#include "el3.h"

using namespace el3;

//XXX: i18n?

static const char* err_strings[] = {
	"No error.",
	"Invalid token.",
	"Bracket does not match.",
	"'->' token can only appear once per block.",
	"Only symbols can appear before '->' token.",
	"'->' token cannot appear outside a block."
};

void Status::print(){
	//TODO: proper filename, line + char number
	fprintf(stderr, "%s:%d:%d: Error: %s\n", "<stdin>", 0, 0, err_strings[errcode]);
}

void el3::token_print(Token t){
	fprintf(stderr, "[%s", token_name_full(t));

	if(t.type == TKN_NUMBER){
		fprintf(stderr, ": %d", t.num.num);		
	}

	if(t.type == TKN_STRING || t.type == TKN_ID || t.type == TKN_SYMBOL){
		fprintf(stderr, ": %.*s", t.str.len, t.str.str);
	}

	if(t.type == TKN_FUNC){
		if(t.func.type == FN_BLOCK){
			fprintf(stderr, ": (BLOCK @ [%d:%d])", t.func.block_start, t.func.block_end);
		} else
		if(t.func.type == FN_NATIVE){
			fprintf(
				stderr,
				": (NATIVE [%.*s])",
				t.func.native->name.sizei(),
				t.func.native->name.data()
			);
		} else {
			fprintf(stderr, ": (NIL)");
		}
	}

	if(t.type == TKN_LIST){
		if(t.list.type == LIST_STACK){
			fprintf(stderr, ": (STACK)");
		} else
		if(t.list.type == LIST_NATIVE){
			fprintf(stderr, ": (NATIVE)");
		} else {
			fprintf(stderr, ": (NIL)");
		}
	}

	if(t.type == TKN_STACK_FRAME){
		fprintf(stderr, ": (%zu)", t.frame.num);
	}
	fprintf(stderr, "]\n");
}



