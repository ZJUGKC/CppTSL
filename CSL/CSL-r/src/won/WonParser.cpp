/*
** Xin YUAN, 2021, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include "WonParser.h"

#include "base/WonDef.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

//rules
const RULEELEMENT g_Rules[] = {
//=============================================================================
//value : WON_TK_STRING
{ WON_NT_value, LA_NULL }, { WON_TK_STRING, WON_PARSER_ACT_TK_STRING }, { TK_NULL, LA_NULL },
//value : WON_TK_NUMBER_INTEGER
{ WON_NT_value, LA_NULL }, { WON_TK_NUMBER_INTEGER, WON_PARSER_ACT_TK_NUMBER_INTEGER }, { TK_NULL, LA_NULL },
//value : WON_TK_NUMBER_FLOAT
{ WON_NT_value, LA_NULL }, { WON_TK_NUMBER_FLOAT, WON_PARSER_ACT_TK_NUMBER_FLOAT }, { TK_NULL, LA_NULL },
//value : WON_TK_NUMBER_HEXADECIMAL
{ WON_NT_value, LA_NULL }, { WON_TK_NUMBER_HEXADECIMAL, WON_PARSER_ACT_TK_NUMBER_HEXADECIMAL }, { TK_NULL, LA_NULL },
//value : WON_TK_BOOLEAN_TRUE
{ WON_NT_value, LA_NULL }, { WON_TK_BOOLEAN_TRUE, WON_PARSER_ACT_TK_BOOLEAN_TRUE }, { TK_NULL, LA_NULL },
//value : WON_TK_BOOLEAN_FALSE
{ WON_NT_value, LA_NULL }, { WON_TK_BOOLEAN_FALSE, WON_PARSER_ACT_TK_BOOLEAN_FALSE }, { TK_NULL, LA_NULL },
//value : WON_TK_KEY_NULL
{ WON_NT_value, LA_NULL }, { WON_TK_KEY_NULL, WON_PARSER_ACT_TK_KEY_NULL }, { TK_NULL, LA_NULL },
//value : WON_TK_LCURLY pair_list WON_TK_RCURLY
{ WON_NT_value, LA_NULL }, { WON_TK_LCURLY, WON_PARSER_ACT_TK_LCURLY }, { WON_NT_pair_list, LA_NULL }, { WON_TK_RCURLY, WON_PARSER_ACT_TK_RCURLY }, { TK_NULL, LA_NULL },
//pair_list : TK_EPSILON
{ WON_NT_pair_list, LA_NULL }, { TK_EPSILON, LA_NULL }, { TK_NULL, LA_NULL },
//pair_list : pair pair_list_tail
{ WON_NT_pair_list, LA_NULL }, { WON_NT_pair, LA_NULL }, { WON_NT_pair_list_tail, LA_NULL }, { TK_NULL, LA_NULL },
//pair_list_tail : WON_TK_COMMA pair pair_list_tail
{ WON_NT_pair_list_tail, LA_NULL }, { WON_TK_COMMA, WON_PARSER_ACT_end_pair }, { WON_NT_pair, LA_NULL }, { WON_NT_pair_list_tail, LA_NULL }, { TK_NULL, LA_NULL },
//pair_list_tail : TK_EPSILON
{ WON_NT_pair_list_tail, WON_PARSER_ACT_end_pair }, { TK_EPSILON, LA_NULL }, { TK_NULL, LA_NULL },
//pair : WON_TK_STRING WON_TK_COLON value
{ WON_NT_pair, LA_NULL }, { WON_TK_STRING, WON_PARSER_ACT_key }, { WON_TK_COLON, LA_NULL }, { WON_NT_value, LA_NULL }, { TK_NULL, LA_NULL },
//value : WON_TK_LBRACKET value_list WON_TK_RBRACKET
{ WON_NT_value, LA_NULL }, { WON_TK_LBRACKET, WON_PARSER_ACT_TK_LBRACKET }, { WON_NT_value_list, LA_NULL }, { WON_TK_RBRACKET, WON_PARSER_ACT_TK_RBRACKET }, { TK_NULL, LA_NULL },
//value_list : TK_EPSILON
{ WON_NT_value_list, LA_NULL }, { TK_EPSILON, LA_NULL }, { TK_NULL, LA_NULL },
//value_list : value value_list_tail
{ WON_NT_value_list, LA_NULL }, { WON_NT_value, LA_NULL }, { WON_NT_value_list_tail, LA_NULL }, { TK_NULL, LA_NULL },
//value_list_tail : WON_TK_COMMA value value_list_tail
{ WON_NT_value_list_tail, LA_NULL }, { WON_TK_COMMA, LA_NULL }, { WON_NT_value, LA_NULL }, { WON_NT_value_list_tail, LA_NULL }, { TK_NULL, LA_NULL },
//value_list_tail : TK_EPSILON
{ WON_NT_value_list_tail, LA_NULL }, { TK_EPSILON, LA_NULL }, { TK_NULL, LA_NULL },
//=============================================================================
//end
{ TK_NULL, LA_NULL }
};

// WonParserHelper

bool WonParserHelper::CreateTable(std::shared_ptr<RdaTable>& spTable)
{
	spTable = std::make_shared<RdaTable>();
	return spTable->Generate(g_Rules, WON_TK_MAX);
}

void WonParserHelper::InitActions(RdParser& parser, WonTokenInfo& info)
{
	parser.ClearActions();
	//TK_STRING
	parser.AddAction(WON_PARSER_ACT_TK_STRING, get_TkStringAction(info));
	//TK_NUMBER_INTEGER
	parser.AddAction(WON_PARSER_ACT_TK_NUMBER_INTEGER, get_TkNumberIntegerAction(info));
	//TK_NUMBER_FLOAT
	parser.AddAction(WON_PARSER_ACT_TK_NUMBER_FLOAT, get_TkNumberFloatAction(info));
	//TK_NUMBER_HEXADECIMAL
	parser.AddAction(WON_PARSER_ACT_TK_NUMBER_HEXADECIMAL, get_TkNumberHexadecimalAction(info));
	//TK_BOOLEAN_TRUE
	parser.AddAction(WON_PARSER_ACT_TK_BOOLEAN_TRUE, get_TkBooleanTrueAction(info));
	//TK_BOOLEAN_FALSE
	parser.AddAction(WON_PARSER_ACT_TK_BOOLEAN_FALSE, get_TkBooleanFalseAction(info));
	//TK_KEY_NULL
	parser.AddAction(WON_PARSER_ACT_TK_KEY_NULL, get_TkKeyNullAction(info));
	//TK_LCURLY
	parser.AddAction(WON_PARSER_ACT_TK_LCURLY, get_TkLcurlyAction(info));
	//TK_RCURLY
	parser.AddAction(WON_PARSER_ACT_TK_RCURLY, get_TkRcurlyAction(info));
	//end pair
	parser.AddAction(WON_PARSER_ACT_end_pair, get_EndPairAction(info));
	//key
	parser.AddAction(WON_PARSER_ACT_key, get_KeyAction(info));
	//TK_LBRACKET
	parser.AddAction(WON_PARSER_ACT_TK_LBRACKET, get_TkLbracketAction(info));
	//TK_RBRACKET
	parser.AddAction(WON_PARSER_ACT_TK_RBRACKET, get_TkRbracketAction(info));
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
