#include "el3.h"
#include "list.h"

using namespace el3;

static void stdlib_add(Context& ctx){
	int result = 0;

	while(!ctx.stack.frame_empty()){
		Token t = ctx.stack.pop_or_throw(TKN_NUMBER);
		result += t.num.num;
	}

	ctx.stack.push<TokenNumber>(result);
}

static void stdlib_sub(Context& ctx){
	Token init = ctx.stack.pop_or_throw(TKN_NUMBER);
	int result = init.num.num;

	while(!ctx.stack.frame_empty()){
		Token t = ctx.stack.pop_or_throw(TKN_NUMBER);
		result -= t.num.num;
	}

	ctx.stack.push<TokenNumber>(result);
}


static void stdlib_mul(Context& ctx){
	Token init = ctx.stack.pop_or_throw(TKN_NUMBER);
	int result = init.num.num;

	while(!ctx.stack.frame_empty()){
		Token t = ctx.stack.pop_or_throw(TKN_NUMBER);
		result *= t.num.num;
	}

	ctx.stack.push<TokenNumber>(result);
}

static void stdlib_div(Context& ctx){
	Token init = ctx.stack.pop_or_throw(TKN_NUMBER);
	int result = init.num.num;

	while(!ctx.stack.frame_empty()){
		Token t = ctx.stack.pop_or_throw(TKN_NUMBER);
		result /= t.num.num;
	}

	ctx.stack.push<TokenNumber>(result);
}


static void stdlib_cons(Context& ctx){
	Token head = ctx.stack.pop();

	ctx.stack.peek_or_throw(TKN_LIST);

	ctx.stack.push(TKN_LIST_JOIN);
	ctx.stack.push(head);
}

static void stdlib_head(Context& ctx){
	Token t = ctx.stack.pop_or_throw(TKN_LIST);
	ctx.stack.push(list_head(t.list, ctx.stack));
}

static void stdlib_tail(Context& ctx){
	Token t = ctx.stack.pop_or_throw(TKN_LIST);
	ctx.stack.push(list_tail(t.list, ctx.stack));
}

static void stdlib_map(Context& ctx){
	Token list = ctx.stack.pop_or_throw(TKN_LIST);
	Token func = ctx.stack.pop_or_throw(TKN_FUNC);

	ctx.stack.push(TKN_LIST_END);
	list_foreach_rev(list.list, ctx.stack, [&](Token t){
		func_call(func.func, ctx, { t });
	});

	ctx.stack.push(TKN_LIST_START);
}

void el3::stdlib_register_funcs(Context& ctx){
	ctx.add_func("+", &stdlib_add);
	ctx.add_func("-", &stdlib_sub);
	ctx.add_func("*", &stdlib_mul);
	ctx.add_func("/", &stdlib_div);

	ctx.add_func("cons", &stdlib_cons);
	ctx.add_func("head", &stdlib_head);
	ctx.add_func("tail", &stdlib_tail);

	ctx.add_func("map", &stdlib_map);
}
