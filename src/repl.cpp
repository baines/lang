#include "el3.h"
#include <readline/readline.h>
#include <readline/history.h>

int main(int argc, char** argv){

	el3::Context ctx;

	ctx.addNativeFunc("+", [](el3::Stack& stack){
		int result = 0;

		while(!stack.empty()){
			el3::Token t;
		    assert((t = stack.try_pop(el3::TokenType::number)));
			
			result += (int)t.data;
		}

		stack.push(el3::TokenType::number, result);
	});
	
	const char* line = nullptr;
	while((line = readline("> "))){
		add_history(line);

		ctx.run_script(line);
		ctx.clear_stack();
	}
	
	puts("");
	return 0;
}
