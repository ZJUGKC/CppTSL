/*
** Xin YUAN, 2019, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include "WmarkParser.h"

#include "base/WmarkDef.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

//rules
const RULEELEMENT g_Rules[] = {
//=============================================================================
//program : return_list r_body
{ WMARK_NT_program, WMARK_PARSER_ACT_program }, { WMARK_NT_return_list, LA_NULL }, { WMARK_NT_r_body, LA_NULL }, { TK_NULL, LA_NULL },
//program : b_body
{ WMARK_NT_program, WMARK_PARSER_ACT_program }, { WMARK_NT_b_body, LA_NULL }, { TK_NULL, LA_NULL },
//r_body : b_body
{ WMARK_NT_r_body, LA_NULL }, { WMARK_NT_b_body, LA_NULL }, { TK_NULL, LA_NULL },
//r_body : TK_EPSILON
{ WMARK_NT_r_body, LA_NULL }, { TK_EPSILON, LA_NULL }, { TK_NULL, LA_NULL },
//b_body : block_element be_tail
{ WMARK_NT_b_body, LA_NULL }, { WMARK_NT_block_element, LA_NULL }, { WMARK_NT_be_tail, LA_NULL }, { TK_NULL, LA_NULL },
//block_element : WMARK_TK_COMMENT
{ WMARK_NT_block_element, LA_NULL }, { WMARK_TK_COMMENT, WMARK_PARSER_ACT_TK_COMMENT }, { TK_NULL, LA_NULL },
//block_element : line_element be_tail
{ WMARK_NT_block_element, WMARK_PARSER_ACT_block_element }, { WMARK_NT_line_element, LA_NULL }, { WMARK_NT_be_tail, LA_NULL }, { TK_NULL, LA_NULL },
//be_tail : WMARK_TK_RETURN ber_tail
{ WMARK_NT_be_tail, LA_NULL }, { WMARK_TK_RETURN, LA_NULL }, { WMARK_NT_ber_tail, LA_NULL }, { TK_NULL, LA_NULL },
//be_tail : TK_EPSILON
{ WMARK_NT_be_tail, LA_NULL }, { TK_EPSILON, LA_NULL }, { TK_NULL, LA_NULL },
//ber_tail : line_element be_tail
{ WMARK_NT_ber_tail, LA_NULL }, { WMARK_NT_line_element, LA_NULL }, { WMARK_NT_be_tail, LA_NULL }, { TK_NULL, LA_NULL },
//ber_tail : return_list berr_tail
{ WMARK_NT_ber_tail, LA_NULL }, { WMARK_NT_return_list, LA_NULL }, { WMARK_NT_berr_tail, LA_NULL }, { TK_NULL, LA_NULL },
//ber_tail : TK_EPSILON
{ WMARK_NT_ber_tail, LA_NULL }, { TK_EPSILON, LA_NULL }, { TK_NULL, LA_NULL },
//berr_tail : b_body
{ WMARK_NT_berr_tail, WMARK_PARSER_ACT_berr_tail }, { WMARK_NT_b_body, LA_NULL }, { TK_NULL, LA_NULL },
//berr_tail : TK_EPSILON
{ WMARK_NT_berr_tail, LA_NULL }, { TK_EPSILON, LA_NULL }, { TK_NULL, LA_NULL },
//return_list : WMARK_TK_RETURN return_list_tail
{ WMARK_NT_return_list, LA_NULL }, { WMARK_TK_RETURN, LA_NULL }, { WMARK_NT_return_list_tail, LA_NULL }, { TK_NULL, LA_NULL },
//return_list_tail : WMARK_TK_RETURN return_list_tail
{ WMARK_NT_return_list_tail, LA_NULL }, { WMARK_TK_RETURN, LA_NULL }, { WMARK_NT_return_list_tail, LA_NULL }, { TK_NULL, LA_NULL },
//return_list_tail : TK_EPSILON
{ WMARK_NT_return_list_tail, LA_NULL }, { TK_EPSILON, LA_NULL }, { TK_NULL, LA_NULL },
//line_element : WMARK_TK_TEXT
{ WMARK_NT_line_element, LA_NULL }, { WMARK_TK_TEXT, WMARK_PARSER_ACT_TK_TEXT }, { TK_NULL, LA_NULL },
//line_element : WMARK_TK_INDENT
{ WMARK_NT_line_element, LA_NULL }, { WMARK_TK_INDENT, WMARK_PARSER_ACT_TK_INDENT }, { TK_NULL, LA_NULL },
//=============================================================================
//end
{ TK_NULL, LA_NULL }
};

// WmarkParserHelper

bool WmarkParserHelper::CreateTable(std::shared_ptr<RdaTable>& spTable)
{
	spTable = std::make_shared<RdaTable>();
	return spTable->Generate(g_Rules, WMARK_TK_MAX);
}

void WmarkParserHelper::InitActions(RdParser& parser, RdParserActionMetaData& data)
{
	parser.ClearActions();
	//accepted
	parser.SetAcceptedAction(get_AcceptedAction(data));
	//program
	parser.AddAction(WMARK_PARSER_ACT_program, get_ProgramAction(data));
	//TK_COMMENT
	parser.AddAction(WMARK_PARSER_ACT_TK_COMMENT, get_TkCommentAction(data));
	//block_element
	parser.AddAction(WMARK_PARSER_ACT_block_element, get_BlockElementAction(data));
	//berr_tail
	parser.AddAction(WMARK_PARSER_ACT_berr_tail, get_BerrTailAction(data));
	//TK_TEXT
	parser.AddAction(WMARK_PARSER_ACT_TK_TEXT, get_TkTextAction(data));
	//TK_INDENT
	parser.AddAction(WMARK_PARSER_ACT_TK_INDENT, get_TkIndentAction(data));
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
