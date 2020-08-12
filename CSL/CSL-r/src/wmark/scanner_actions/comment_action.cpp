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

// CommentAction

RdScannerAction WmarkScannerHelper::get_CommentAction()
{
	return [](std::istream& stm, RdActionStack& stk, RdToken& token)->bool
			{
				int iState = 1;
				do {
					//get a character
					char ch;
					stm.get(ch);
					if( stm.eof() ) {
						if( iState < 4 )
							token.uID = WMARK_TK_TEXT;
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
						if( ch != '!' ) {
							stk.push(WMARK_SCANNER_TEXT_ACTION);
							return true;
						}
						iState = 2;
						break;
					case 2:
						if( ch != '-' ) {
							stk.push(WMARK_SCANNER_TEXT_ACTION);
							return true;
						}
						iState = 3;
						break;
					case 3:
						if( ch != '-' ) {
							stk.push(WMARK_SCANNER_TEXT_ACTION);
							return true;
						}
						iState = 4;
						break;
					case 4:
						if( ch == '-' ) {
							iState = 5;
						}
						else if( ch == '\n' ) {
							token.infoEnd.uRow ++;
							token.infoEnd.uCol = 0;
						}
						else if( ch == '\r' ) {
							stm.get(ch);
							if( stm.eof() ) {
								token.uID = TK_ERROR;
								token.infoEnd.uRow ++;
								token.infoEnd.uCol = 0;
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
					case 5:
						if( ch != '-' )
							iState = 4;
						else
							iState = 6;
						break;
					case 6:
						if( ch != '>' )
							iState = 4;
						else
							iState = 7;
						break;
					default:
						return false;
					}
				} while( iState != 7 );

				//comment
				token.uID = WMARK_TK_COMMENT;
				return true;
			};
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
