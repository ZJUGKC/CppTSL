/*
** Xin YUAN, 2021, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////
#ifndef __WON_PARSER_H__
#define __WON_PARSER_H__
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

// WonParserHelper

class WonParserHelper
{
public:
	static bool CreateTable(std::shared_ptr<RdaTable>& spTable);
	static void InitActions(RdParser& parser, WonTokenInfo& info);

private:
	static RdParserAction get_TkStringAction(WonTokenInfo& info);
	static RdParserAction get_TkNumberIntegerAction(WonTokenInfo& info);
	static RdParserAction get_TkNumberFloatAction(WonTokenInfo& info);
	static RdParserAction get_TkNumberHexadecimalAction(WonTokenInfo& info);
	static RdParserAction get_TkBooleanTrueAction(WonTokenInfo& info);
	static RdParserAction get_TkBooleanFalseAction(WonTokenInfo& info);
	static RdParserAction get_TkKeyNullAction(WonTokenInfo& info);
	static RdParserAction get_TkLcurlyAction(WonTokenInfo& info);
	static RdParserAction get_TkRcurlyAction(WonTokenInfo& info);
	static RdParserAction get_EndPairAction(WonTokenInfo& info);
	static RdParserAction get_KeyAction(WonTokenInfo& info);
	static RdParserAction get_TkLbracketAction(WonTokenInfo& info);
	static RdParserAction get_TkRbracketAction(WonTokenInfo& info);
};

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
