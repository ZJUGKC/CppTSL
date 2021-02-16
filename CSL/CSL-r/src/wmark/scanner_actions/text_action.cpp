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

// TextAction

RdScannerAction WmarkScannerHelper::get_TextAction()
{
	return [](std::istream& stm, uint32_t& next, RdToken& token)->bool
			{
				do {
					//get a character
					char ch;
					stm.get(ch);
					if( stm.eof() ) {
						token.uID = WMARK_TK_TEXT;
						return true;
					}
					if( !stm.good() )
						return false;

					if( ch == '\r' || ch == '\n' ) {
						stm.unget();
						break;
					}

					token.strToken += ch;
					token.infoEnd.uCol ++;
				} while( true );

				token.uID = WMARK_TK_TEXT;
				return true;
			};
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
