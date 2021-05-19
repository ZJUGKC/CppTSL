/*
** Xin YUAN, 2021, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include "WonScanner.h"

#include "base/WonDef.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

// WonScannerHelper

void WonScannerHelper::InitActions(RdScanner& rds)
{
	rds.ClearActions();
	//add
	rds.AddAction(WON_SCANNER_TK_ACTION, get_TkAction());
	rds.AddAction(WON_SCANNER_COMMENT_ACTION, get_CommentAction());
	rds.AddAction(WON_SCANNER_STRING_ACTION, get_StringAction());
	rds.AddAction(WON_SCANNER_ZERO_NUMBER_ACTION, get_ZeroNumberAction());
	rds.AddAction(WON_SCANNER_NONZERO_NUMBER_ACTION, get_NonzeroNumberAction());
	rds.AddAction(WON_SCANNER_EXP_NUMBER_ACTION, get_ExpNumberAction());
	rds.AddAction(WON_SCANNER_HEX_NUMBER_ACTION, get_HexNumberAction());
}

//key
bool scanner_process_key(const char* key1, const uint32_t uTK, std::istream& stm, uint32_t& next, RdToken& token)
{
	const char* p = key1;
	do {
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

		if( ch != *p ) {
			token.uID = TK_ERROR;
			return true;
		}

		p ++;
		if( *p == '\0' )
			break;
	} while( true );

	token.uID = uTK;
	return true;
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
