#ifndef EL3_SYMBOL_TABLE_H_
#define EL3_SYMBOL_TABLE_H_
#include <map>
#include <cassert>
#include <cstring>
#include "tokens.h"
#include <functional>

namespace el3 {

	struct Symbol {

		Symbol() = default;

		template<class F>
		Symbol(F&& func) : native_func(std::forward<F>(func)){}

		void call(Stack& s){
			native_func(s);
		}

		std::function<void(Stack&)> native_func;
		//TODO: more stuff
	};

	struct SymbolTable {
		typedef std::pair<const char*, size_t> ID;

		void add(const char* name, size_t name_len, const Symbol& val){
			syms[std::make_pair(name, name_len)] = val;
		}

		Symbol* lookup(const char* name, size_t name_len){
			auto it = syms.find(std::make_pair(name, name_len));
			return it == syms.end() ? nullptr : &(it->second);
		}

		struct IDComp {
			bool operator()(const ID& a, const ID& b) const {
				return a.second == b.second
					? memcmp(a.first, b.first, a.second) < 0
					: a.second < b.second;
			}
		};

		std::map<ID, Symbol, IDComp> syms;
	};

};

#endif
