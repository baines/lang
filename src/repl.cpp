#include "el3.h"
#include <readline/readline.h>
#include <readline/history.h>

int main(int argc, char** argv){

	el3::Context ctx;

	ctx.add_func("+", [](el3::Stack& stack){
		int result = 0;

		while(!stack.empty()){
			el3::Token t = stack.try_pop(el3::TokenType::number);
			assert(t);
			
			result += t.get<int>();
		}

		stack.push(el3::TokenType::number, result);
	});

	read_history("repl_history.txt");

	char* line = nullptr;
	while((line = readline("> "))){
		add_history(line);
		write_history("repl_history.txt");
		
		ctx.run_script(line);
		ctx.clear_stack();

		free(line);
	}

	
	puts("");
	return 0;
}
