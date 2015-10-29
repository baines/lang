#include "el3.h"
#include "symbol_table.h"
#include "stack.h"
#include <vector>
#include <cassert>

using namespace el3;

namespace el3 {

struct TokenStream {

	TokenStream() : tokens(nullptr), ip(0), limit(0){}

	explicit TokenStream(const std::vector<Token>& t)
		: tokens(&t), ip(0), limit(t.size()){}

	TokenStream(const TokenStream& other, size_t new_ip, size_t lim)
		: tokens(other.tokens), ip(new_ip), limit(lim){}

	const std::vector<Token>* tokens;

	const Token& current() const {
		return (*tokens)[ip];
	}

	bool next(){
		return ++ip < limit;
	}

	uint32_t ip, limit;
};

void Context::run_assert(TokenStream& tokens){
	fprintf(stderr, "Got invalid token: %s.\n", token_name_full(tokens.current()));
	abort();
}

void Context::run_stack_push(TokenStream& tokens){
	stack.push(tokens.current());
}

void Context::run_frame_push(TokenStream& tokens){
	stack.frame_push();
}

void Context::run_resolve_id(TokenStream& tokens){
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

void Context::run_func_eval(TokenStream& tokens){

	if(stack.frame_empty()){
		stack.frame_pop();
		return;
	}

	fprintf(stderr, "Pre-args: ");
	stack.debug_print();

	auto it = stack.back_iterate();
	stack.frame_push();

	while(it.count() > 1){
		Token t = it.next();
		
		fprintf(stderr, "Arg: ");
		token_print(t);

		if(t.type == TKN_LIST_END){
			// Find matching list_start + push TokenList that points to this stack location.
			// This should make writing native funcs easier since Lists will be a single
			//  token, but has other complications.
			
			uint32_t end_idx = it.index();
			Token list_elem;

			do {
				list_elem = it.next();
			} while(list_elem.type != TKN_LIST_START);

			stack.push<TokenList>(LIST_STACK, it.index() + 1, end_idx);

		} else {
			stack.push(t);
		}
	}

	Token eval_token = it.next();
	fprintf(stderr, "Eval: ");
	token_print(eval_token);

	fprintf(stderr, "Before eval: ");
	stack.debug_print();

	if(eval_token.type == TKN_FUNC){
		if(eval_token.func.type == FN_NATIVE){
			auto* fn = eval_token.func.native;
			
			fn->name.pass_c_str([](char* name){
				fprintf(stderr, "Calling native func [%s]\n", name);
			});

			fn->ptr(*this);
		}
		if(eval_token.func.type == FN_BLOCK){
			size_t start = eval_token.func.block_start, end = eval_token.func.block_end;
			TokenStream sub_tokens(tokens, start, end);

			stack.frame_push();
			syms.push_scope();
			
			fprintf(stderr, "------ running block ------\n");
			run(sub_tokens);
			fprintf(stderr, "------ end of block ------\n");

			syms.pop_scope();
		}

		// go through stack and convert TokenLists to the actual tokens

		auto it = stack.back_iterate();
		stack.frame_push();

		bool in_list = false;

		while(Token t = it.next()){
			
			if(it.peek().type == TKN_LIST_JOIN){
				stack.push(TKN_LIST_START);
			}

			if(t.type == TKN_LIST){
				if(!in_list){
					stack.push(TKN_LIST_START);
				}

				for(int i = t.list.stack_start; i < t.list.stack_end; ++i){
					stack.push(stack[i]);
				}

				stack.push(TKN_LIST_END);
				in_list = false;

			} else if(t.type == TKN_LIST_JOIN){
				in_list = true;
				assert(it.peek().type == TKN_LIST);
			} else {
				stack.push(t);
			}
		}

		stack.frame_erase();

	} else {
		stack.push(eval_token);
	}

	stack.frame_erase();
	stack.frame_pop();

	fprintf(stderr, "After eval: ");
	stack.debug_print();
}

void Context::run_block_start(TokenStream& tokens){
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

void Context::run_bind_args(TokenStream& tokens){
	Token t;

	while((t = stack.try_pop(TKN_SYMBOL))){
		
		Token binding = stack.pop_under(TKN_STACK_FRAME);

		fprintf(
			stderr,
			"Binding arg %.*s -> %s\n",
			t.sym.len, t.sym.str,
			token_name_full(binding)
		);

		syms.add_token(t, binding);
	}

}

void Context::run(TokenStream& tokens){
	do {
		switch(tokens.current().type){
			case TKN_ID:
				run_resolve_id(tokens);
				break;
			case TKN_NUMBER:
			case TKN_STRING:
			case TKN_SYMBOL:
			case TKN_LIST_START:
			case TKN_LIST_END:
				run_stack_push(tokens);
				break;
			case TKN_ARGS_MARKER:
				run_bind_args(tokens);
				break;
			case TKN_FUNC_START:
				run_frame_push(tokens);
				break;
			case TKN_FUNC_END:
			case TKN_BLOCK_END:
				run_func_eval(tokens);
				break;
			case TKN_BLOCK_START:
				run_block_start(tokens);
				break;
			default:
				run_assert(tokens);
				break;
		}
	} while(tokens.next());
}

Status Context::execute(const std::vector<Token>& tokens){
	TokenStream token_stream(tokens);
	run(token_stream);

	//TODO: report runtime errors
	return no_error;
}

void Context::clear_stack(){
	stack.clear();
}

void Context::run_script(const char* script){

	Status status;

	if(!(status = lex(script, token_vec))){
		status.print();
		return;
	}

	/*
	for(auto& t : token_vec){
		t.debug_print();
	}*/

	if(!(status = parse(token_vec))){
		status.print();
		return;
	}

	if(!(status = execute(token_vec))){
		status.print();
		return;
	}

	puts("\nRESULT:");

	for(auto& t : stack){
		token_print(t);
	}

}

void func_call(TokenFunc func, Context& ctx, std::initializer_list<Token> args){

	ctx.stack.frame_push();

	for(auto i = std::rbegin(args), j = std::rend(args); i != j; ++i){
		ctx.stack.push(*i);
	}

	if(func.type == FN_NATIVE){
		func.native->ptr(ctx);
	}
	if(func.type == FN_BLOCK){
		size_t start = func.block_start, end = func.block_end;
		TokenStream sub_tokens(TokenStream(ctx.token_vec), start, end);

		ctx.stack.frame_push();
		ctx.syms.push_scope();
		
		fprintf(stderr, "------ running block ------\n");
		ctx.run(sub_tokens);
		fprintf(stderr, "------ end of block ------\n");

		ctx.syms.pop_scope();
	}

	auto it = ctx.stack.back_iterate();
	ctx.stack.frame_push();

	bool in_list = false;

	while(Token t = it.next()){
			
		if(it.peek().type == TKN_LIST_JOIN){
			ctx.stack.push(TKN_LIST_START);
		}

		if(t.type == TKN_LIST){
			if(!in_list){
				ctx.stack.push(TKN_LIST_START);
			}

			for(int i = t.list.stack_start; i < t.list.stack_end; ++i){
				ctx.stack.push(ctx.stack[i]);
			}

			ctx.stack.push(TKN_LIST_END);
			in_list = false;

		} else if(t.type == TKN_LIST_JOIN){
			in_list = true;
			assert(it.peek().type == TKN_LIST);
		} else {
			ctx.stack.push(t);
		}
	}

	ctx.stack.frame_erase();
	ctx.stack.frame_pop();
}

}
