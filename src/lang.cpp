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
		fprintf(stderr, "ip: %zu\n", ip);
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

static void run_func_eval(TokenList& tokens, Stack& stack, SymbolTable& syms){
	//TODO: ArgStack adaptor for stack, less copying needed.
	fprintf(stderr, "evaluating func [%s]\n", tokens.current().debug_name());

	TokenType cur_type = tokens.current().type, match_type = TokenType::invalid;

	if(cur_type == TokenType::func_end){
		match_type = TokenType::func_start;
	} else
	if(cur_type == TokenType::block_end){
		match_type = TokenType::block_start;
	}
	assert(match_type != TokenType::invalid);
	assert(!stack.empty());

	//TODO: resolve identifiers in arguments before calling the func.
	//      will require Symbol to embed a Token as well as std::function
	
	Stack sub_stack;

	Token t;
	while(t = stack.pop(), t.type != match_type){
		sub_stack.push(t);
	}

	//TODO: allow evaluation of all types. Use identity function for numbers, strings etc.
	Token eval_token;

	//XXX: when id lookup is done, this will be TokenType::native_func instead of id.
	if((eval_token = sub_stack.try_pop(TokenType::id))){
		Symbol* callee = syms.lookup(eval_token.get<const char*>(), eval_token.size);
		assert(callee);
		fprintf(stderr, "calling native func: [%.*s]\n", (int)eval_token.size, eval_token.get<const char*>());
		callee->call(sub_stack);
	} else
	if((eval_token = sub_stack.try_pop(TokenType::block_marker))){
		sub_stack.push(TokenType::block_start); //FIXME
		size_t start = eval_token.get<size_t>(), end = eval_token.size;
		TokenList sub_tokens(tokens, start, end);
		fprintf(stderr, "------ running block ------\n");
		run(sub_tokens, sub_stack, syms);
		fprintf(stderr, "------ end of block ------\n");
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
	assert(curly_count == 0);

	// save the location of the start + end of this block in a block_marker token.
	// when it is evaluated, all the skipped tokens inside will be evaluated.
	stack.push(TokenType::block_marker, block_start_ip, tokens.ip + 1);
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
	run_func_eval,   // block_end
	run_assert       // native_func
};

static void run(TokenList& tokens, Stack& stack, SymbolTable& syms){
	do {
		size_t index = static_cast<size_t>(tokens.current().type);
		fn_table[index](tokens, stack, syms);
	} while(tokens.next());
}

void Context::execute(vector<Token>& token_vec){
	TokenList tokens(token_vec);
	run(tokens, this->stack, this->sym_tab);
}

void Context::run_script(const char* script){

	std::vector<Token> tokens;
	lex(script, tokens);

	for(auto& t : tokens){
		t.debug_print();
	}

	//TODO: simple parsing before executing to make sure brackets are well formed etc.

	execute(tokens);

	puts("\nRESULT:");

	for(auto& t : stack){
		t.debug_print();
	}

}
