/*
** Xin YUAN, 2019, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////
#ifndef __WMARK_PARSER_H__
#define __WMARK_PARSER_H__
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

// WmarkParserHelper

class WmarkParserHelper
{
public:
	static bool CreateTable(std::shared_ptr<RdaTable>& spTable);
	static void InitActions(RdParser& parser, RdParserActionMetaData* pData);
	static void Start(RdParser& parser);

private:
	static RdParserAction get_AcceptedAction(RdParserActionMetaData* pData);
	static RdParserAction get_BerrTailAction(RdParserActionMetaData* pData);
	static RdParserAction get_BlockElementAction(RdParserActionMetaData* pData);
	static RdParserAction get_ProgramAction(RdParserActionMetaData* pData);
	static RdParserAction get_TkCommentAction(RdParserActionMetaData* pData);
	static RdParserAction get_TkIndentAction(RdParserActionMetaData* pData);
	static RdParserAction get_TkTextAction(RdParserActionMetaData* pData);
};

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
