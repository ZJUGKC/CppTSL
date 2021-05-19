/*
** Xin YUAN, 2021, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////
#ifndef __WON_DEF_H__
#define __WON_DEF_H__
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

//Action IDs for scanner
enum {
	WON_SCANNER_TK_ACTION = LA_START,
	WON_SCANNER_COMMENT_ACTION,
	WON_SCANNER_STRING_ACTION,
	WON_SCANNER_ZERO_NUMBER_ACTION,
	WON_SCANNER_NONZERO_NUMBER_ACTION,
	WON_SCANNER_EXP_NUMBER_ACTION,
	WON_SCANNER_HEX_NUMBER_ACTION
};

//Token IDs
enum {
	WON_TK_STRING = TK_START,
	WON_TK_NUMBER_INTEGER,
	WON_TK_NUMBER_FLOAT,
	WON_TK_NUMBER_HEXADECIMAL,
	WON_TK_LCURLY,
	WON_TK_RCURLY,
	WON_TK_COLON,
	WON_TK_COMMA,
	WON_TK_LBRACKET,
	WON_TK_RBRACKET,
	WON_TK_BOOLEAN_TRUE,
	WON_TK_BOOLEAN_FALSE,
	WON_TK_KEY_NULL,
	WON_TK_MAX
};

//NT
enum {
	WON_NT_value = 501,
	WON_NT_pair_list,
	WON_NT_pair,
	WON_NT_pair_list_tail,
	WON_NT_value_list,
	WON_NT_value_list_tail
};

//actions for parser
enum {
	WON_PARSER_ACT_TK_STRING = LA_START,
	WON_PARSER_ACT_TK_NUMBER_INTEGER,
	WON_PARSER_ACT_TK_NUMBER_FLOAT,
	WON_PARSER_ACT_TK_NUMBER_HEXADECIMAL,
	WON_PARSER_ACT_TK_BOOLEAN_TRUE,
	WON_PARSER_ACT_TK_BOOLEAN_FALSE,
	WON_PARSER_ACT_TK_KEY_NULL,
	WON_PARSER_ACT_TK_LCURLY,
	WON_PARSER_ACT_TK_RCURLY,
	WON_PARSER_ACT_end_pair,
	WON_PARSER_ACT_key,
	WON_PARSER_ACT_TK_LBRACKET,
	WON_PARSER_ACT_TK_RBRACKET
};

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
