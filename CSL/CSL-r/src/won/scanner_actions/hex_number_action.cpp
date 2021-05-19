/*
** Xin YUAN, 2021, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include "../WonScanner.h"

#include "../base/WonDef.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

// HexNumberAction

RdScannerAction WonScannerHelper::get_HexNumberAction()
{
	return [](std::istream& stm, uint32_t& next, RdToken& token)->bool
			{
				int iState = 1;
				do {
					//get a character
					char ch;
					stm.get(ch);
					if( stm.eof() ) {
						if( iState != 1 && iState == ((iState >> 1) << 1) + 1 )
							token.uID = WON_TK_NUMBER_HEXADECIMAL;
						else
							token.uID = TK_ERROR;
						return true;
					}
					if( !stm.good() )
						return false;
					token.strToken += ch;
					token.infoEnd.uCol ++;

					if( ::isxdigit(ch) ) {
						iState ++;
					}
					else {
						if( iState != 1 && iState == ((iState >> 1) << 1) + 1 ) {
							token.strToken.erase(token.strToken.size() - 1, 1);
							token.infoEnd.uCol --;
							stm.unget();
							token.uID = WON_TK_NUMBER_HEXADECIMAL;
							return true;
						}
						token.uID = TK_ERROR;
						return true;
					}
				} while( iState != 17 ); //up to 8 bytes --> 16 digits

				token.uID = WON_TK_NUMBER_HEXADECIMAL;
				return true;
			};
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
