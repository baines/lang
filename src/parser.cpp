#include "el3.h"

using namespace el3;

static TokenType matching_bracket(TokenType t){
	switch(t){
		case TKN_FUNC_START:  return TKN_FUNC_END;
		case TKN_BLOCK_START: return TKN_BLOCK_END;
		case TKN_LIST_START:  return TKN_LIST_END;
		case TKN_FUNC_END:    return TKN_FUNC_START;
		case TKN_BLOCK_END:   return TKN_BLOCK_START;
		case TKN_LIST_END:    return TKN_LIST_START;
		default:                     return TKN_INVALID;	
	}
}

static Status validate_args_marker(const std::vector<Token>& tokens, ssize_t index){
	for(ssize_t i = index - 1; i > 0; --i){
		Token t = tokens[i];

		//NOTE: It is permissible for a block with no args to have an arg marker.

		if(t.type == TKN_BLOCK_START){
			return no_error;
		}
		if(t.type == TKN_ARGS_MARKER){
			return Status(EL3_ERR_MULTIPLE_ARG_DECL, tokens[index]);
		}
		if(t.type != TKN_SYMBOL){
			return Status(EL3_ERR_NON_SYMBOL_ARG, tokens[i]);
		}
	}

	return Status(EL3_ERR_ARG_MARKER_OUTSIDE_BLOCK, tokens[index]);
}

Status Context::parse(const std::vector<Token>& tokens){

	Stack brackets(tokens.size());

	for(size_t i = 0, j = tokens.size(); i < j; ++i){

		Token t = tokens[i];

		switch(t.type){
			case TKN_FUNC_START:
			case TKN_BLOCK_START:
			case TKN_LIST_START:
				brackets.push(t);
				break;
			case TKN_FUNC_END:
			case TKN_BLOCK_END:
			case TKN_LIST_END: {
				Token match = brackets.pop();
				if(match.type != matching_bracket(t.type)){
					return Status(EL3_ERR_BRACKET_MISMATCH, t);
				}
				break;
			}
			case TKN_ARGS_MARKER: {
				auto status = validate_args_marker(tokens, i);
				if(!status) return status;
				break;
			}
		}
	}

	return brackets.frame_empty() ? no_error : Status(EL3_ERR_BRACKET_MISMATCH, brackets.pop());
}
