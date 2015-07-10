#ifndef EL3_H_
#define EL3_H_
#include "tokens.h"
#include "stack.h"
#include "symbol_table.h"
#include <vector>

namespace el3 {

	struct Context {
		Context() = default;
		
		template<size_t N, class T>
		void addNativeFunc(const char(&name)[N], T&& func){
			sym_tab.add(name, N-1, Symbol(func));
		}

		void run_script(const char* script);

		void lex(const char* script, std::vector<Token>& tokens);
		void execute(std::vector<Token>& tokens);

	private:
		Stack stack;
		SymbolTable sym_tab;
	};
}

#endif
