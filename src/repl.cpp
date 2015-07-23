#include "el3.h"

int main(int argc, char** argv){

	const char* script = "(+ 1 (+ 2 3))";

	if(argc < 2){
		fprintf(stderr, "WARNING: no script given, using default.\n");
	} else {
		script = argv[1];
	}	

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

	ctx.run_script(script);

	return 0;

}
