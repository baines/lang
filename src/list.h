#ifndef EL3_LIST_H_
#define EL3_LIST_H_

namespace el3 {

// FIXME: all these functions need to transform LIST_START / LIST_END tokens
//        into TKN_LIST 

bool list_empty(TokenList list){
	return list.stack_start == list.stack_end;
}

Token list_head(TokenList list, const Stack& s){ 
	if(list_empty(list)){
		return TKN_NIL;
	} else {
		return s[list.stack_start];
	}
}

TokenList list_tail(TokenList list, const Stack& s){
	if(list_empty(list)){
		return TokenList { LIST_NIL };
	} else {
		list.stack_start++;
		return list;
	}
}

template<class F>
void list_foreach(TokenList list, const Stack& s, F&& func){
	while(!list_empty(list)){
		Token t = s[list.stack_start++];
		func(t);
	}
}

template<class F>
void list_foreach_rev(TokenList list, const Stack& s, F&& func){
	while(!list_empty(list)){
		Token t = s[--list.stack_end];
		func(t);
	}
}

template<class F>
void list_foreach_kv(TokenList list, const Stack& s, F&& func){
	while(!list_empty(list)){
		Token k = s[list.stack_start++];
		Token v = list_empty(list) ? TKN_NIL : s[list.stack_start++];

		func(k, v);
	}
}

}

#endif

