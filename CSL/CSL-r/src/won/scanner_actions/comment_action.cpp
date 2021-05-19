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

// CommentAction

RdScannerAction WonScannerHelper::get_CommentAction()
{
	return [](std::istream& stm, uint32_t& next, RdToken& token)->bool
			{
				int iState = 1;
				do {
					//get a character
					char ch;
					stm.get(ch);
					if( stm.eof() ) {
						assert( iState < 4 );
						token.uID = TK_ERROR;
						return true;
					}
					if( !stm.good() )
						return false;
					token.strToken += ch;
					token.infoEnd.uCol ++;

					switch( iState ) {
					case 1:
						if( ch != '*' ) {
							token.uID = TK_ERROR;
							return true;
						}
						iState = 2;
						break;
					case 2:
						if( ch == '*' ) {
							iState = 3;
						}
						else if( ch == '\n' ) {
							token.infoEnd.uRow ++;
							token.infoEnd.uCol = 0;
						}
						else if( ch == '\r' ) {
							stm.get(ch);
							if( stm.eof() ) {
								token.infoEnd.uRow ++;
								token.infoEnd.uCol = 0;
								token.uID = TK_ERROR;
								return true;
							}
							if( !stm.good() )
								return false;
							token.infoEnd.uRow ++;
							token.infoEnd.uCol = 0;
							if( ch == '\n' )
								token.strToken += ch;
							else
								stm.unget();
						}
						break;
					case 3:
						if( ch == '/' )
							iState = 4;
						else
							iState = 2;
						break;
					default:
						return false;
					}
				} while( iState != 4 );

				token.uID = TK_NULL;
				return true;
			};
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
