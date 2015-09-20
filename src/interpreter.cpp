#include "el3.h"
#include "symbol_table.h"
#include "stack.h"
#include <vector>
#include <cassert>

using namespace el3;
using std::vector;

struct TokenList {
	explicit TokenList(const vector<Token>& t)
		: tokens(t), ip(0), limit(t.size()){}

	TokenList(const TokenList& other, size_t new_ip, size_t lim)
		: tokens(other.tokens), ip(new_ip), limit(lim){}

	const vector<Token>& tokens;

	const Token& current() const {
		return tokens[ip];
	}

	bool next(){
		return ++ip < limit;
	}

	size_t ip, limit;
};

static void run(TokenList& tokens, Stack& stack, SymbolTable& syms);

static void run_assert(TokenList& tokens, Stack& stack, SymbolTable& syms){
	fprintf(stderr, "Got invalid token: %s.\n", tokens.current().debug_name());
	abort();
}

static void run_stack_push(TokenList& tokens, Stack& stack, SymbolTable& syms){
	stack.push(tokens.current());
}

static void run_resolve_id(TokenList& tokens, Stack& stack, SymbolTable& syms){
	Token t = syms.lookup(tokens.current());
	fprintf(
		stderr,
		"Lookup id %.*s -> %s\n",
		(int)tokens.current().size,
		tokens.current().get<const char*>(),
		t.debug_name()
	);

	stack.push(t);
}

static void run_func_eval(TokenList& tokens, Stack& stack, SymbolTable& syms){

	TokenType cur_type = tokens.current().type,
	          end_type = (cur_type == TokenType::func_end)
	           	       ? TokenType::func_start
	                   : (cur_type == TokenType::block_end)
	                   ? TokenType::block_start
	                   : TokenType::invalid;

	assert(end_type != TokenType::invalid);
	assert(!stack.empty());
	
	Stack sub_stack;
	Token t;

	while(t = stack.pop(), t.type != end_type){
		sub_stack.push(t);
	}

	//TODO: allow evaluation of all types. Use identity function for numbers, strings etc.
	Token eval_token = sub_stack.pop();

	switch(eval_token.type){
		case TokenType::native_func: {
			auto* fn = eval_token.get<NativeFunc*>();
			
			fn->name.pass_c_str([](char* name){
				fprintf(stderr, "Calling native func [%s]\n", name);
			});

			fn->ptr(sub_stack);

			break;
		}
		case TokenType::block_marker: {
			sub_stack.push(TokenType::block_start);
			size_t start = eval_token.get<size_t>(), end = eval_token.size;
			TokenList sub_tokens(tokens, start, end);

			syms.push_scope();
			
			fprintf(stderr, "------ running block ------\n");
			run(sub_tokens, sub_stack, syms);
			fprintf(stderr, "------ end of block ------\n");

			syms.pop_scope();

			break;
		}
		default: {
			sub_stack.push(eval_token);
			break;
		}
	}

	stack.append(sub_stack);
}

static void run_block_start(TokenList& tokens, Stack& stack, SymbolTable& syms){
	size_t block_start_ip = tokens.ip + 1;

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

	// save the location of the start + end of this block in a block_marker token.
	// when it is evaluated, all the skipped tokens inside will be evaluated.
	stack.push(TokenType::block_marker, block_start_ip, tokens.ip + 1);
}

static void run_bind_args(TokenList& tokens, Stack& stack, SymbolTable& syms){
	Token t;

	while((t = stack.try_pop(TokenType::symbol))){
		
		Token binding = stack.pop_under(TokenType::block_start);

		fprintf(
			stderr,
			"Binding arg %.*s -> %s\n",
			(int)t.size, t.get<const char*>(),
			binding.debug_name()
		);

		syms.add_token(t, binding);
	}

}

static void (*fn_table[])(TokenList& tokens, Stack& stack, SymbolTable& syms) = {
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

static void run(TokenList& tokens, Stack& stack, SymbolTable& syms){
	do {
		size_t index = static_cast<size_t>(tokens.current().type);
		fn_table[index](tokens, stack, syms);
	} while(tokens.next());
}

Status Context::execute(const vector<Token>& token_vec){
	TokenList tokens(token_vec);
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
		t.debug_print();
	}

}
