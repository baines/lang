#ifndef EL3_H_
#define EL3_H_
#include "common.h"
#include "stack.h"
#include "symbol_table.h"
#include <list>

namespace el3 {

	struct Context {
		Context() = default;
		
		template<class F>
		void add_func(const alt::StrRef& name, F&& func){
			native_funcs.push_back(NativeFunc{ name, std::forward<F>(func) });
			sym_tab.add_native_func(name, native_funcs.back());
		}

		void run_script(const char* script);
		void clear_stack();

	private:
		Status lex(const char* script, std::vector<Token>& tokens);
		Status parse(const std::vector<Token>& tokens);
		Status execute(const std::vector<Token>& tokens);
		
		Stack stack;
		SymbolTable sym_tab;
		std::list<NativeFunc> native_funcs;
	};

}

#endif
