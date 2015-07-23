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
		fprintf(stderr, "ip: %zu\n", ip);
		return ++ip < tokens.size();
	}

	size_t ip;
};

static void run_assert(TokenList& tokens, Stack& stack, SymbolTable& syms){
	fprintf(stderr, "Got invalid token: %s.\n", tokens.current().debug_name());
	abort();
}

static void run_stack_push(TokenList& tokens, Stack& stack, SymbolTable& syms){
	fprintf(stderr, "Pushing token: [%s]\n", tokens.current().debug_name());
	stack.push(tokens.current());
}

static void run_func_eval(TokenList& tokens, Stack& stack, SymbolTable& syms){
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

static void run_block_start(TokenList& tokens, Stack& stack, SymbolTable& syms){
	stack.push(TokenType::block_marker, tokens.ip);

	// skip all tokens until matching block_end token, they shouldn't be evaluated now.
	int curly_count = 1;
	while(curly_count > 0 && tokens.next()){
		TokenType cur_type = tokens.current().type;
		if(cur_type == TokenType::block_start){
			++curly_count;
		} else 
		if(cur_type == TokenType::block_end){
		   	--curly_count;
		}
	}

	assert(curly_count == 0);
}

static void (*fn_table[])(TokenList& tokens, Stack& stack, SymbolTable& syms) = {
	run_assert,      // invalid
	run_stack_push,  // id
	run_stack_push,  // number
	run_stack_push,  // string
	run_stack_push,  // func_start
	run_func_eval,   // func_end
	run_assert,      // list_start
	run_assert,      // list_end
	run_block_start, // block_start
	run_assert,      // block_end
	run_assert       // native_func
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
		t.debug_print();
	}

	execute(tokens);

	puts("RESULT:");

	for(auto& t : stack){
		t.debug_print();
	}

}
