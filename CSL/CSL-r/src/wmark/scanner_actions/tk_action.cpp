/*
** Xin YUAN, 2019, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include "../WmarkScanner.h"

#include "../base/WmarkDef.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

// TkAction

RdScannerAction WmarkScannerHelper::get_TkAction()
{
	return [](std::istream& stm, uint32_t& next, RdToken& token)->bool
			{
				//get a character
				char ch;
				stm.get(ch);
				if( stm.eof() ) {
					token.uID = TK_END_OF_EVENT;
					return true;
				}
				if( !stm.good() )
					return false;
				token.strToken += ch;

				//return
				if( ch == '\n' ) {
					token.uID = WMARK_TK_RETURN;
					token.infoEnd.uRow ++;
					token.infoEnd.uCol = 0;
					return true;
				}
				if( ch == '\r' ) {
					stm.get(ch);
					if( stm.eof() ) {
						token.uID = WMARK_TK_RETURN;
						token.infoEnd.uRow ++;
						token.infoEnd.uCol = 0;
						return true;
					}
					if( !stm.good() )
						return false;
					token.infoEnd.uRow ++;
					token.infoEnd.uCol = 0;
					if( ch == '\n' ) {
						token.strToken += ch;
						token.uID = WMARK_TK_RETURN;
						return true;
					}
					stm.unget();
					token.uID = WMARK_TK_RETURN;
					return true;
				}

				token.infoEnd.uCol ++;

				//indent
				if( ch == '\t' ) {
					token.uID = WMARK_TK_INDENT;
					return true;
				}

				//<
				if( ch == '<' ) {
					next = WMARK_SCANNER_COMMENT_ACTION;
					return true;
				}

				//others
				next = WMARK_SCANNER_TEXT_ACTION;

				return true;
			};
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
