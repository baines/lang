#ifndef EL3_SYMBOL_TABLE_H_
#define EL3_SYMBOL_TABLE_H_
#include <map>
#include <cassert>
#include <cstring>
#include <functional>
#include "tokens.h"
#include "altlib/alt.h"

//TODO: use hashes instead of actual strings for better lookup speed.

namespace el3 {

	using native_fn = std::function<void(Stack&)>;
	
	struct SymbolTable {

		SymbolTable() : syms(), frame_num(0){}

		void add_native_func(const alt::StrRef& name, const native_fn& func){
			assert(frame_num == 0 && "Native funcs should be added before running a script.");
			syms.emplace_back(name, Token{TokenType::native_func, &func}, frame_num);
		}

		void add_token(Token id, Token val){
			if(id.type != TokenType::id && id.type != TokenType::symbol){
				return;
			}

			const alt::StrRef name(id.get<const char*>(), id.size);
			syms.emplace_back(name, val, frame_num);
		}

		Token lookup(Token id){
			if(id.type != TokenType::id && id.type != TokenType::symbol){
				return TokenType::invalid;
			}

			const alt::StrRef name(id.get<const char*>(), id.size);
			auto it = std::find(syms.rbegin(), syms.rend(), name);

			return it == syms.rend()
			       ? TokenType::invalid
			       : it->token;
		}

		void push_scope(){
			++frame_num;
		}

		void pop_scope(){
			--frame_num;
			syms.erase(
				std::remove_if(syms.begin(), syms.end(), [&](const Entry& e){
					return e.frame_num > frame_num;
				})
			);
		}

		struct Entry {
			Entry() = default;

			Entry(const alt::StrRef& s, Token t, uint32_t fnum)
			: name(s), token(t), frame_num(fnum){}

			alt::StrRef name;
			Token token;
			uint32_t frame_num;

			bool operator==(const alt::StrRef& str) const {
				return name == str;
			}
		};

		std::vector<Entry> syms;
		uint32_t frame_num;
	};

};

#endif
