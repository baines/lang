#include "el3.h"
#include <editline/readline.h>
#include <editline/history.h>

int main(int argc, char** argv){

	el3::Context ctx;

	read_history("repl_history.txt");

	char* line = nullptr;
	while((line = readline("> "))){
		add_history(line);
		write_history("repl_history.txt");
		
		ctx.run_script(line);
		ctx.reset();

		free(line);
	}

	puts("");
	return 0;
}
