#ifndef EL3_STACK_H_
#define EL3_STACK_H_
#include <vector>
#include <cassert>
#include <algorithm>
#include "tokens.h"

namespace el3 {

	struct StackBackIter {
	
		StackBackIter(std::vector<Token>& tokens, size_t limit)
		: tokens(tokens)
		, cursor(tokens.size() - 1)
		, limit(limit){

		}

		size_t count() const {
			return cursor - limit;
		}

		Token next(){
			if(count()){
				return tokens[cursor--];
			} else {
				return TKN_INVALID;
			}
		}

		Token peek(){
			if(count()){
				return tokens[cursor];
			} else {
				return TKN_INVALID;
			}
		}

		uint32_t index(){
			return cursor + 1;
		}

		size_t cursor, limit;
		std::vector<Token>& tokens;
	};

	struct Stack {

		Stack(size_t sz)
		: tokens()
		, frame_num(0) {
			tokens.reserve(sz);
		}

		Stack() : Stack(256){}

		Token pop() {
			if(tokens.empty() || tokens.back().type == TKN_STACK_FRAME){
				return TKN_INVALID;
			}

			Token t = tokens.back();
			tokens.pop_back();
			return t;
		}

		Token try_pop(TokenType type){
			if(tokens.empty() || tokens.back().type == TKN_STACK_FRAME){
				return TKN_INVALID;
			}

			Token t = tokens.back();
			if(t.type == type){
				tokens.pop_back();
				return t;
			} else {
				return TKN_INVALID;
			}
		}

		Token pop_or_throw(TokenType type){
 			if(tokens.empty() || tokens.back().type == TKN_STACK_FRAME){
				throw TypeMismatch{ type, TKN_NIL };
			}

			Token t = tokens.back();
			if(t.type == type){
				tokens.pop_back();
				return t;
			} else {
				throw TypeMismatch{ type, t.type };
			}
		}

		Token peek_or_throw(TokenType type){
 			if(tokens.empty() || tokens.back().type == TKN_STACK_FRAME){
				throw TypeMismatch{ type, TKN_NIL };
			}

			Token t = tokens.back();
			if(t.type == type){
				return t;
			} else {
				throw TypeMismatch{ type, t.type };
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
			fprintf(stderr, "[");
			for(auto& t : tokens){
				fprintf(stderr, "%s ", token_name(t));
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

		void frame_push(){
			push<TokenStackFrame>(frame_num++);
		}

		void frame_pop(){
			auto it = std::find(tokens.rbegin(), tokens.rend(), TKN_STACK_FRAME);

			if(it != tokens.rend()){
				tokens.erase(it.base() - 1);
				frame_num--;
			}
		}

		void frame_erase(){
			auto i = std::find(tokens.rbegin(), tokens.rend(), TKN_STACK_FRAME);
			if(i == tokens.rend()) return;

			auto j = std::find(i + 1, tokens.rend(), TKN_STACK_FRAME);
			if(j == tokens.rend()) return;

			tokens.erase(j.base() - 1, i.base() - 1);

			i = std::find(tokens.rbegin(), tokens.rend(), TKN_STACK_FRAME);
			assert(i != tokens.rend());

			i->frame.num--;
			frame_num--;
		}

		StackBackIter back_iterate(){
			size_t lim = 0;
			auto it = std::find(tokens.rbegin(), tokens.rend(), TKN_STACK_FRAME);
			if(it != tokens.rend()){
				lim = &(*it) - tokens.data();
			}

			return StackBackIter(tokens, lim);
		}

		bool frame_empty() const {
			return tokens.empty() || tokens.back().type == TKN_STACK_FRAME;
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

		Token operator[](size_t i) const {
			return tokens[i];
		}

		std::vector<Token> tokens;
		size_t frame_num;
	};

}

#endif
