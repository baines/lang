#include "el3.h"

using namespace el3;

static TokenType matching_bracket(TokenType t){
	switch(t){
		case TokenType::func_start:  return TokenType::func_end;
		case TokenType::block_start: return TokenType::block_end;
		case TokenType::list_start:  return TokenType::list_end;
		case TokenType::func_end:    return TokenType::func_start;
		case TokenType::block_end:   return TokenType::block_start;
		case TokenType::list_end:    return TokenType::list_start;
		default:                     return TokenType::invalid;	
	}
}

static Status validate_args_marker(const std::vector<Token>& tokens, ssize_t index){
	for(ssize_t i = index - 1; i > 0; --i){
		Token t = tokens[i];

		//NOTE: It is permissible for a block with no args to have an arg marker.

		if(t.type == TokenType::block_start){
			return no_error;
		}
		if(t.type == TokenType::args_marker){
			return Status(EL3_ERR_MULTIPLE_ARG_DECL, tokens[index]);
		}
		if(t.type != TokenType::symbol){
			return Status(EL3_ERR_NON_SYMBOL_ARG, tokens[i]);
		}
	}

	return Status(EL3_ERR_ARG_MARKER_OUTSIDE_BLOCK, tokens[index]);
}

Status Context::parse(const std::vector<Token>& tokens){

	Stack brackets;

	for(size_t i = 0, j = tokens.size(); i < j; ++i){

		Token t = tokens[i];

		switch(t.type){
			case TokenType::func_start:
			case TokenType::block_start:
			case TokenType::list_start:
				brackets.push(t);
				break;
			case TokenType::func_end:
			case TokenType::block_end:
			case TokenType::list_end: {
				Token match = brackets.pop();
				if(match.type != matching_bracket(t.type)){
					return Status(EL3_ERR_BRACKET_MISMATCH, t);
				}
				break;
			}
			case TokenType::args_marker: {
				auto status = validate_args_marker(tokens, i);
				if(!status) return status;
				break;
			}
		}
	}

	return no_error;
}
