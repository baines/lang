#ifndef EL3_H_
#define EL3_H_
#include "tokens.h"
#include "stack.h"
#include "symbol_table.h"
#include <list>

namespace el3 {

	struct Context {
		Context() = default;
		
		template<class F>
		void add_func(const alt::StrRef& name, F&& func){
			native_funcs.push_back(std::forward<F>(func));
			sym_tab.add_native_func(name, native_funcs.back());
		}

		void run_script(const char* script);

		void clear_stack();

		bool lex(const char* script, std::vector<Token>& tokens);
		void execute(std::vector<Token>& tokens);

	private:
		Stack stack;
		SymbolTable sym_tab;
		std::list<std::function<void(Stack&)>> native_funcs;
	};
}

#endif
