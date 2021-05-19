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

// NonzeroNumberAction

RdScannerAction WonScannerHelper::get_NonzeroNumberAction()
{
	return [](std::istream& stm, uint32_t& next, RdToken& token)->bool
			{
				do {
					//get a character
					char ch;
					stm.get(ch);
					if( stm.eof() ) {
						token.uID = WON_TK_NUMBER_INTEGER;
						return true;
					}
					if( !stm.good() )
						return false;

					if( !::isdigit(ch) ) {
						stm.unget();
						break;
					}

					token.strToken += ch;
					token.infoEnd.uCol ++;
				} while( true );

				next = WON_SCANNER_ZERO_NUMBER_ACTION;
				return true;
			};
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
