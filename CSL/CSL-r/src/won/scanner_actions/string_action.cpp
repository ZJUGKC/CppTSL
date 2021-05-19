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

// StringAction

RdScannerAction WonScannerHelper::get_StringAction()
{
	return [](std::istream& stm, uint32_t& next, RdToken& token)->bool
			{
				int iState = 1;
				do {
					//get a character
					char ch;
					stm.get(ch);
					if( stm.eof() ) {
						token.uID = TK_ERROR;
						return true;
					}
					if( !stm.good() )
						return false;
					token.strToken += ch;
					token.infoEnd.uCol ++;

					switch( iState ) {
					case 1:
						if( ch == '\"' ) {
							iState = 7;
						}
						else if( ch == '\\' ) {
							iState = 2;
						}
						else if( ch >= 0x00 && ch <= 0x1F ) {
							token.uID = TK_ERROR;
							return true;
						}
						break;
					case 2:
						if( ::strchr("\"\\/bfnrt", ch) != NULL ) {
							iState = 1;
						}
						else if( ch == 'u' ) {
							iState = 3;
						}
						else {
							token.uID = TK_ERROR;
							return true;
						}
						break;
					case 3:
						if( ::isxdigit(ch) ) {
							iState = 4;
						}
						else {
							token.uID = TK_ERROR;
							return true;
						}
						break;
					case 4:
						if( ::isxdigit(ch) ) {
							iState = 5;
						}
						else {
							token.uID = TK_ERROR;
							return true;
						}
						break;
					case 5:
						if( ::isxdigit(ch) ) {
							iState = 6;
						}
						else {
							token.strToken.erase(token.strToken.size() - 1, 1);
							token.infoEnd.uCol --;
							stm.unget();
							iState = 1;
						}
						break;
					case 6:
						if( ::isxdigit(ch) ) {
							iState = 1;
						}
						else {
							token.uID = TK_ERROR;
							return true;
						}
						break;
					default:
						return false;
					}
				} while( iState != 7 );

				token.uID = WON_TK_STRING;
				return true;
			};
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
