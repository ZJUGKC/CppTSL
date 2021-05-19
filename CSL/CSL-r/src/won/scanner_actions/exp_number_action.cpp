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

// ExpNumberAction

RdScannerAction WonScannerHelper::get_ExpNumberAction()
{
	return [](std::istream& stm, uint32_t& next, RdToken& token)->bool
			{
				int iState = 1;
				do {
					//get a character
					char ch;
					stm.get(ch);
					if( stm.eof() ) {
						if( iState == 3 )
							token.uID = WON_TK_NUMBER_FLOAT;
						else
							token.uID = TK_ERROR;
						return true;
					}
					if( !stm.good() )
						return false;
					token.strToken += ch;
					token.infoEnd.uCol ++;

					switch( iState ) {
					case 1:
						if( ch == '+' || ch == '-' ) {
							iState = 2;
						}
						else if( ::isdigit(ch) ) {
							iState = 3;
						}
						else {
							token.uID = TK_ERROR;
							return true;
						}
						break;
					case 2:
						if( ::isdigit(ch) ) {
							iState = 3;
						}
						else {
							token.uID = TK_ERROR;
							return true;
						}
						break;
					case 3:
						if( !::isdigit(ch) ) {
							iState = 4;
						}
						break;
					default:
						return false;
					}
				} while( iState != 4 );

				token.strToken.erase(token.strToken.size() - 1, 1);
				token.infoEnd.uCol --;
				stm.unget();

				token.uID = WON_TK_NUMBER_FLOAT;
				return true;
			};
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
