#include "el3.h"
#include "symbol_table.h"
#include "stack.h"
#include <vector>
#include <cassert>

using namespace el3;
using std::vector;

struct TokenList {
	explicit TokenList(const vector<Token>& t)
		: tokens(t), ip(0){}

	const vector<Token>& tokens;

	const Token& current() const {
		return tokens[ip];
	}

	bool next(){
		fprintf(stderr, "IP: %zu\n", ip);
		return ++ip < tokens.size();
	}

	size_t ip;
};

static void run_assert(const TokenList& tokens, Stack& stack, SymbolTable& syms){
	fprintf(stderr, "Got invalid token type: %d. This should never happen.\n", tokens.current().type);
	abort();
}

static void run_stack_push(const TokenList& tokens, Stack& stack, SymbolTable& syms){
	fprintf(stderr, "Pushing token: %d\n", tokens.current().type);
	stack.push(tokens.current());
}

static void run_func_eval(const TokenList& tokens, Stack& stack, SymbolTable& syms){
	//TODO: ArgStack adaptor for stack, less copying needed.

	Stack args;
	Token t;

	assert(!stack.empty());

	//TODO: resolve identifiers in arguments before calling the func.
	//      will require Symbol to embed a Token as well as std::function
	
	while(t = stack.pop(), t.type != TokenType::func_start){
		args.push(t);
	}

	Token callee_id = args.try_pop(TokenType::id);
	assert(callee_id);

	auto* name = reinterpret_cast<const char*>(callee_id.data);
	Symbol* callee_fn = syms.lookup(name, callee_id.size);
	assert(callee_fn);

	callee_fn->call(args);

	stack.append(args);
}

static void (*fn_table[])(const TokenList& tokens, Stack& stack, SymbolTable& syms) = {
	run_assert,
	run_stack_push,
	run_stack_push,
	run_stack_push,
	run_stack_push,
	run_func_eval
};

void Context::execute(vector<Token>& tokens){
	TokenList tlist(tokens);

	do {
		size_t index = static_cast<size_t>(tlist.current().type);
		fn_table[index](tlist, stack, sym_tab);
	} while(tlist.next());
}

void Context::run_script(const char* script){

	std::vector<Token> tokens;
	lex(script, tokens);

	for(auto& t : tokens){
		token_print(t);
	}

	execute(tokens);

	puts("RESULT:");

	for(auto& t : stack){
		token_print(t);
	}

}
