#ifndef EL3_STACK_H_
#define EL3_STACK_H_
#include <vector>
#include <cassert>
#include <algorithm>
#include "tokens.h"

namespace el3 {

	struct Stack {

		Token pop() {
			if(tokens.empty()) return TKN_INVALID;

			Token t = tokens.back();
			tokens.pop_back();
			return t;
		}

		Token try_pop(TokenType type){
			if(tokens.empty()) return TKN_INVALID;

			Token t = tokens.back();
			if(t.type == type){
				tokens.pop_back();
				return t;
			} else {
				return TKN_INVALID;
			}
		}

		Token pop_under(TokenType type){
			auto it = std::find(tokens.rbegin(), tokens.rend(), type);

			if(it == tokens.rend() || ++it == tokens.rend()){
				return TKN_INVALID;
			} else {
				Token t = *it;
				tokens.erase(it.base()-1);
				return t;
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
				fprintf(stderr, "%s, ", token_name(t));
			}
			fprintf(stderr, "]\n");
		}

		void push(const Token& t){
			assert(t.type != TKN_INVALID);
			tokens.push_back(t);
		}

		template<class T, class... Args>
		void push(Args&&... args){
			T t = { static_cast<Args>(args)... };
			push(Token(t));
		}

		bool empty() const {
			return tokens.empty();
		}

		void clear() {
			tokens.clear();
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
