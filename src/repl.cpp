#include "el3.h"

int main(void){

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

	ctx.run_script("(+ 1 1 1 (+ 2 3))");

	return 0;

}
