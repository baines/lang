#ifndef EL3_H_
#define EL3_H_
#include "common.h"
#include "stack.h"
#include "symbol_table.h"
#include <list>

namespace el3 {

	struct Context {
		Context(){
			stdlib_register_funcs(*this);
		}
		
		template<class F>
		void add_func(const alt::StrRef& name, F&& func){
			native_funcs.push_back(NativeFunc{ name, std::forward<F>(func) });
			syms.add_native_func(name, native_funcs.back());
		}

		void run_script(const char* script);
		void clear_stack();

		Stack stack;
		SymbolTable syms;
		std::vector<Token> token_vec;

		void run(TokenStream&);

	private:
		Status lex(const char* script, std::vector<Token>& tokens);
		Status parse(const std::vector<Token>& tokens);
		Status execute(const std::vector<Token>& tokens);

		void run_assert(TokenStream&);
		void run_resolve_id(TokenStream&);
		void run_stack_push(TokenStream&);
		void run_bind_args(TokenStream&);
		void run_frame_push(TokenStream&);
		void run_func_eval(TokenStream&);
		void run_block_start(TokenStream&);
		
		std::list<NativeFunc> native_funcs;
	};

}

#endif
