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

// TkAction

RdScannerAction WonScannerHelper::get_TkAction()
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
					token.infoEnd.uRow ++;
					token.infoEnd.uCol = 0;
					token.uID = TK_NULL;
					return true;
				}
				if( ch == '\r' ) {
					stm.get(ch);
					if( stm.eof() ) {
						token.infoEnd.uRow ++;
						token.infoEnd.uCol = 0;
						token.uID = TK_NULL;
						return true;
					}
					if( !stm.good() )
						return false;
					token.infoEnd.uRow ++;
					token.infoEnd.uCol = 0;
					if( ch == '\n' ) {
						token.strToken += ch;
						token.uID = TK_NULL;
						return true;
					}
					stm.unget();
					token.uID = TK_NULL;
					return true;
				}

				token.infoEnd.uCol ++;

				//space
				if( ch == ' ' || ch == '\t' ) {
					do {
						stm.get(ch);
						if( stm.eof() ) {
							token.uID = TK_NULL;
							return true;
						}
						if( !stm.good() )
							return false;
						if( ch != ' ' && ch != '\t' ) {
							stm.unget();
							break;
						}
						token.strToken += ch;
						token.infoEnd.uCol ++;
					} while( true );
					token.uID = TK_NULL;
					return true;
				}

				//comment
				if( ch == '/' ) {
					next = WON_SCANNER_COMMENT_ACTION;
					return true;
				}

				//string
				if( ch == '\"') {
					next = WON_SCANNER_STRING_ACTION;
					return true;
				}

				//number
				if( ch == '-' ) {
					stm.get(ch);
					if( stm.eof() ) {
						token.uID = TK_ERROR;
						return true;
					}
					if( !stm.good() )
						return false;
					token.strToken += ch;
					token.infoEnd.uCol ++;
					if( !::isdigit(ch) ) {
						token.uID = TK_ERROR;
						return true;
					}
				}
				if( ch == '0' ) {
					stm.get(ch);
					if( stm.eof() ) {
						token.uID = WON_TK_NUMBER_INTEGER;
						return true;
					}
					if( !stm.good() )
						return false;
					if( ch == 'x' ) {
						token.strToken += ch;
						token.infoEnd.uCol ++;
						if( token.strToken[0] == '-' ) {
							token.uID = TK_ERROR;
							return true;
						}
						next = WON_SCANNER_HEX_NUMBER_ACTION;
						return true;
					}
					stm.unget();
					next = WON_SCANNER_ZERO_NUMBER_ACTION;
					return true;
				}
				if( ch >= '1' && ch <= '9' ) {		
					next = WON_SCANNER_NONZERO_NUMBER_ACTION;
					return true;
				}

				//separators
				if( ch == '{' ) {
					token.uID = WON_TK_LCURLY;
					return true;
				}
				if( ch == '}' ) {
					token.uID = WON_TK_RCURLY;
					return true;
				}
				if( ch == ':' ) {
					token.uID = WON_TK_COLON;
					return true;
				}
				if( ch == ',' ) {
					token.uID = WON_TK_COMMA;
					return true;
				}
				if( ch == '[' ) {
					token.uID = WON_TK_LBRACKET;
					return true;
				}
				if( ch == ']' ) {
					token.uID = WON_TK_RBRACKET;
					return true;
				}

				//values
				if( ch == 't' )
					return scanner_process_key("rue", WON_TK_BOOLEAN_TRUE, stm, next, token);
				if( ch == 'f' )
					return scanner_process_key("alse", WON_TK_BOOLEAN_FALSE, stm, next, token);
				if( ch == 'n' )
					return scanner_process_key("ull", WON_TK_KEY_NULL, stm, next, token);

				//others
				token.uID = TK_ERROR;
				return true;
			};
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
