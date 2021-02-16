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
	static void InitActions(RdParser& parser, RdParserActionMetaData& data);

private:
	static RdParserAction get_AcceptedAction(RdParserActionMetaData& data);
	static RdParserAction get_BerrTailAction(RdParserActionMetaData& data);
	static RdParserAction get_BlockElementAction(RdParserActionMetaData& data);
	static RdParserAction get_ProgramAction(RdParserActionMetaData& data);
	static RdParserAction get_TkCommentAction(RdParserActionMetaData& data);
	static RdParserAction get_TkIndentAction(RdParserActionMetaData& data);
	static RdParserAction get_TkTextAction(RdParserActionMetaData& data);
};

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
