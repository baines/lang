#ifndef EL3_SYMBOL_TABLE_H_
#define EL3_SYMBOL_TABLE_H_
#include <map>
#include <cassert>
#include <cstring>
#include <functional>
#include "common.h"
#include "tokens.h"
#include "alt_str.h"

//TODO: use hashes instead of actual strings for better lookup speed.

namespace el3 {

	struct SymbolTable {

		SymbolTable() : syms(), frame_num(0){}

		void add_native_func(const alt::StrRef& name, NativeFunc& func){
			assert(frame_num == 0 && "Native funcs should be added before running a script.");
			syms.emplace_back(name, TokenFunc{FN_NATIVE, 0, 0, &func}, frame_num);
		}

		void add_token(Token id, Token val){
			if(id.type != TKN_ID && id.type != TKN_SYMBOL){
				return;
			}

			const alt::StrRef name(id.id.str, id.id.len);
			syms.emplace_back(name, val, frame_num);
		}

		bool update_token(Token id, Token val){
			if(id.type != TKN_ID && id.type != TKN_SYMBOL){
				return false;
			}

			const alt::StrRef name(id.id.str, id.id.len);
			auto it = std::find(syms.rbegin(), syms.rend(), name);
			
			if(it == syms.rend()){
				return false;
			}

			it->token = val;
			return true;
		}

		Token lookup(Token id){
			if(id.type != TKN_ID && id.type != TKN_SYMBOL){
				return TKN_INVALID;
			}

			const alt::StrRef name(id.id.str, id.id.len);
			auto it = std::find(syms.rbegin(), syms.rend(), name);

			return it == syms.rend()
			       ? TKN_INVALID
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
				}),
				syms.end()
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

		const Entry* begin() const { return syms.data(); }
		const Entry* end()   const { return syms.data() + syms.size(); }

		std::vector<Entry> syms;
		uint32_t frame_num;
	};

};

#endif
