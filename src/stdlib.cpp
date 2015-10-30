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

static void stdlib_assign(Context& ctx){
	Token key = ctx.stack.pop_or_throw(TKN_SYMBOL);
	Token val = ctx.stack.pop();

	if(!ctx.syms.update_token(key, val)){
		throw UndeclaredIdentifier{ key.id };
	}
}

static void stdlib_gt(Context& ctx){
	Token a = ctx.stack.pop_or_throw(TKN_NUMBER);
	Token b = ctx.stack.pop_or_throw(TKN_NUMBER);
	ctx.stack.push<TokenNumber>(a.num.num > b.num.num ? 1 : 0);
}

static void stdlib_lt(Context& ctx){
	Token a = ctx.stack.pop_or_throw(TKN_NUMBER);
	Token b = ctx.stack.pop_or_throw(TKN_NUMBER);
	ctx.stack.push<TokenNumber>(a.num.num < b.num.num ? 1 : 0);
}

static void stdlib_eq(Context& ctx){
	Token a = ctx.stack.pop_or_throw(TKN_NUMBER);
	Token b = ctx.stack.pop_or_throw(TKN_NUMBER);
	ctx.stack.push<TokenNumber>(a.num.num == b.num.num ? 1 : 0);
}

static void stdlib_ge(Context& ctx){
	Token a = ctx.stack.pop_or_throw(TKN_NUMBER);
	Token b = ctx.stack.pop_or_throw(TKN_NUMBER);
	ctx.stack.push<TokenNumber>(a.num.num >= b.num.num ? 1 : 0);
}

static void stdlib_le(Context& ctx){
	Token a = ctx.stack.pop_or_throw(TKN_NUMBER);
	Token b = ctx.stack.pop_or_throw(TKN_NUMBER);
	ctx.stack.push<TokenNumber>(a.num.num <= b.num.num ? 1 : 0);
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

static void stdlib_if(Context& ctx){
	Token cond    = ctx.stack.pop_or_throw(TKN_FUNC);
	Token t_true  = ctx.stack.pop();
	Token t_false = ctx.stack.pop();

	func_call(cond.func, ctx, {});
	Token res = ctx.stack.pop();

	bool choice = true;
	switch(res.type){
		case TKN_NUMBER: choice = res.num.num != 0; break;
		case TKN_NIL:    choice = false;            break;
	}

	ctx.stack.push(choice ? t_true : t_false);
}

static void stdlib_while(Context& ctx){
	Token cond = ctx.stack.pop_or_throw(TKN_FUNC);
	Token loop = ctx.stack.pop_or_throw(TKN_FUNC);

	do {
		func_call(cond.func, ctx, {});
		Token res = ctx.stack.pop();
		if(res.type == TKN_NIL || (res.type == TKN_NUMBER && res.num.num == 0)){
			break;
		}
		func_call(loop.func, ctx, {});
	} while(true);
}

static void stdlib_let(Context& ctx){
	Token locals = ctx.stack.pop_or_throw(TKN_LIST);
	Token block  = ctx.stack.pop_or_throw(TKN_FUNC);

	ctx.syms.push_scope();

	list_foreach_kv(locals.list, ctx.stack, [&](Token key, Token val){
		if(key.type != TKN_SYMBOL){
			throw TypeMismatch{ TKN_SYMBOL, key.type };
		}

		ctx.syms.add_token(key, val);
	});

	func_call(block.func, ctx, {});

	ctx.syms.pop_scope();
}

/*

// FIXME: will break when putting lists in symbol table because the stack frame they refer to
//        gets removed when the function returns...
//
//        also breaks if tokens are cleared since the name strings are stored there.

static void stdlib_def(Context& ctx){
	Token key = ctx.stack.pop_or_throw(TKN_SYMBOL);
	Token val = ctx.stack.pop();

	ctx.syms.add_token(key, val);
}
*/

static void echo_helper(Context& ctx, Token t){
	switch(t.type){
		case TKN_STRING:
		case TKN_ID:
		case TKN_SYMBOL:
			printf("%.*s\n", t.str.len, t.str.str);
			break;
		case TKN_NUMBER:
			printf("%d\n", t.num.num);
			break;
		case TKN_NIL:
			puts("(nil)");
			break;
		case TKN_LIST:
			list_foreach(t.list, ctx.stack, [&](Token tt){
				echo_helper(ctx, tt);
			});
			break;
		default:
			puts("(haven't implemented echo for this type of token :x)");
			break;
	}
}

static void stdlib_echo(Context& ctx){
	while(!ctx.stack.frame_empty()){
		Token t = ctx.stack.pop();
		echo_helper(ctx, t);
	}
}

static void stdlib_dump(Context& ctx){
	for(const auto& e : ctx.syms){
		printf("%.*s: ", e.name.sizei(), e.name.data());
		token_print(e.token, stdout);
	}
}

void el3::stdlib_register_funcs(Context& ctx){
	ctx.add_func("+", &stdlib_add);
	ctx.add_func("-", &stdlib_sub);
	ctx.add_func("*", &stdlib_mul);
	ctx.add_func("/", &stdlib_div);

	ctx.add_func("=", &stdlib_assign);

	ctx.add_func(">",  &stdlib_gt);
	ctx.add_func("<",  &stdlib_lt);
	ctx.add_func("==", &stdlib_eq);
	ctx.add_func(">=", &stdlib_ge);
	ctx.add_func("<=", &stdlib_le);

	ctx.add_func("cons", &stdlib_cons);
	ctx.add_func("head", &stdlib_head);
	ctx.add_func("tail", &stdlib_tail);
	ctx.add_func("map",  &stdlib_map);

	ctx.add_func("if",    &stdlib_if);
	ctx.add_func("while", &stdlib_while);

	ctx.add_func("let", &stdlib_let);
//	ctx.add_func("def", &stdlib_def);

	ctx.add_func("echo", &stdlib_echo);
	ctx.add_func("dump", &stdlib_dump);
}
