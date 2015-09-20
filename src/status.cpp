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
