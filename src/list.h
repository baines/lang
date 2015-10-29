#ifndef EL3_LIST_H_
#define EL3_LIST_H_

namespace el3 {

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
void list_foreach_rev(TokenList list, const Stack& s, F&& func){
	while(!list_empty(list)){
		fprintf(stderr, "IN LIST:");
		Token t = s[--list.stack_end];
		token_print(t);

		func(t);
	}
}

}

#endif

