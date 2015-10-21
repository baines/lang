#include "el3.h"
#include "symbol_table.h"
#include "stack.h"
#include <vector>
#include <cassert>

using namespace el3;
using std::vector;

struct TokenStream {
	explicit TokenStream(const vector<Token>& t)
		: tokens(t), ip(0), limit(t.size()){}

	TokenStream(const TokenStream& other, size_t new_ip, size_t lim)
		: tokens(other.tokens), ip(new_ip), limit(lim){}

	const vector<Token>& tokens;

	const Token& current() const {
		return tokens[ip];
	}

	bool next(){
		return ++ip < limit;
	}

	uint32_t ip, limit;
};

static void run(TokenStream& tokens, Stack& stack, SymbolTable& syms);

static void run_assert(TokenStream& tokens, Stack& stack, SymbolTable& syms){
	fprintf(stderr, "Got invalid token: %s.\n", token_name_full(tokens.current()));
	abort();
}

static void run_stack_push(TokenStream& tokens, Stack& stack, SymbolTable& syms){
	stack.push(tokens.current());
}

static void run_resolve_id(TokenStream& tokens, Stack& stack, SymbolTable& syms){
	Token t = syms.lookup(tokens.current());
	fprintf(
		stderr,
		"Lookup id %.*s -> %s\n",
		tokens.current().id.len,
		tokens.current().id.str,
		token_name_full(t)
	);

	stack.push(t);
}

static void run_func_eval(TokenStream& tokens, Stack& stack, SymbolTable& syms){

	TokenType cur_type = tokens.current().type,
	          end_type = (cur_type == TKN_FUNC_END)
	           	       ? TKN_FUNC_START
	                   : (cur_type == TKN_BLOCK_END)
	                   ? TKN_BLOCK_START
	                   : TKN_INVALID;

	assert(end_type != TKN_INVALID);
	assert(!stack.empty());
	
	Stack sub_stack;
	Token t;

	while(t = stack.pop(), t.type != end_type){
		sub_stack.push(t);
	}

	Token eval_token = sub_stack.pop();

	if(eval_token.type == TKN_FUNC){
		if(eval_token.func.type == FN_NATIVE){
			auto* fn = eval_token.func.native;
			
			fn->name.pass_c_str([](char* name){
				fprintf(stderr, "Calling native func [%s]\n", name);
			});

			fn->ptr(sub_stack);
		}
		if(eval_token.func.type == FN_BLOCK){
			sub_stack.push(TKN_BLOCK_START);
			size_t start = eval_token.func.block_start, end = eval_token.func.block_end;
			TokenStream sub_tokens(tokens, start, end);

			syms.push_scope();
			
			fprintf(stderr, "------ running block ------\n");
			run(sub_tokens, sub_stack, syms);
			fprintf(stderr, "------ end of block ------\n");

			syms.pop_scope();
		}
	} else {
		sub_stack.push(eval_token);
	}

	stack.append(sub_stack);
}

static void run_block_start(TokenStream& tokens, Stack& stack, SymbolTable& syms){
	uint32_t block_start_ip = tokens.ip + 1;

	// skip all tokens until matching block_end token, they shouldn't be evaluated now.
	int curly_count = 1;
	while(curly_count > 0 && tokens.next()){
		TokenType cur_type = tokens.current().type;
		if(cur_type == TKN_BLOCK_START){
			++curly_count;
		} else 
		if(cur_type == TKN_BLOCK_END){
		   	--curly_count;
		}
	}

	// save the location of the start + end of this block in a block_marker token.
	// when it is evaluated, all the skipped tokens inside will be evaluated.
	stack.push<TokenFunc>(FN_BLOCK, block_start_ip, tokens.ip + 1u);
}

static void run_bind_args(TokenStream& tokens, Stack& stack, SymbolTable& syms){
	Token t;

	while((t = stack.try_pop(TKN_SYMBOL))){
		
		Token binding = stack.pop_under(TKN_BLOCK_START);

		fprintf(
			stderr,
			"Binding arg %.*s -> %s\n",
			t.sym.len, t.sym.str,
			token_name_full(binding)
		);

		syms.add_token(t, binding);
	}

}

static void (*fn_table[])(TokenStream& tokens, Stack& stack, SymbolTable& syms) = {
	run_assert,      // invalid
	run_resolve_id,  // id
	run_stack_push,  // number
	run_stack_push,  // string
	run_stack_push,  // symbol
	run_bind_args,   // args_marker
	run_stack_push,  // func_start
	run_func_eval,   // func_end
	run_assert,      // list_start
	run_assert,      // list_end
	run_block_start, // block_start
	run_func_eval,   // block_end
	run_assert,      // native_func
	run_assert,      // block_marker
	run_assert       // num_tokens
};

static void run(TokenStream& tokens, Stack& stack, SymbolTable& syms){
	do {
		size_t index = static_cast<size_t>(tokens.current().type);
		fn_table[index](tokens, stack, syms);
	} while(tokens.next());
}

Status Context::execute(const vector<Token>& token_vec){
	TokenStream tokens(token_vec);
	run(tokens, this->stack, this->sym_tab);

	//TODO: report runtime errors
	return no_error;
}

void Context::clear_stack(){
	stack.clear();
}

void Context::run_script(const char* script){

	Status status;
	std::vector<Token> tokens;

	if(!(status = lex(script, tokens))){
		status.print();
		return;
	}

	/*
	for(auto& t : tokens){
		t.debug_print();
	}*/

	if(!(status = parse(tokens))){
		status.print();
		return;
	}

	if(!(status = execute(tokens))){
		status.print();
		return;
	}

	puts("\nRESULT:");

	for(auto& t : stack){
		token_print(t);
	}

}
