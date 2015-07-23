#ifndef EL3_STACK_H_
#define EL3_STACK_H_
#include <vector>
#include <cassert>
#include "tokens.h"

namespace el3 {

	struct Stack {

		Token pop() {
			if(tokens.empty()) return TokenType::invalid;

			Token t = tokens.back();
			tokens.pop_back();
			return t;
		}

		Token try_pop(TokenType type){
			if(tokens.empty()) return TokenType::invalid;

			Token t = tokens.back();
			if(t.type == type){
				tokens.pop_back();
				return t;
			} else {
				return TokenType::invalid;
			}
		}

		void append(Stack& other){
			for(auto& t : other.tokens){
				push(t);
			}
		}

		void debug_print() const {
			fprintf(stderr, "%p: [", this);
			for(auto& t : tokens){
				fprintf(stderr, "%.2s, ", t.debug_name());
			}
			fprintf(stderr, "]\n");
		}

		void push(const Token& t){
			assert(t.type != TokenType::invalid);
			tokens.push_back(t);
			debug_print();
		}

		void push(TokenType t, const intptr_t data, size_t sz = 0){
			push(Token(t, data, sz));
		}
		
		void push(TokenType t, const void* data, size_t sz = 0){
			push(Token(t, data, sz));
		}

		bool empty() const {
			return tokens.empty();
		}

		typename std::vector<Token>::iterator begin() {
			return tokens.begin();
		}

		typename std::vector<Token>::iterator end() {
			return tokens.end();
		}

		std::vector<Token> tokens;
	};

}

#endif
