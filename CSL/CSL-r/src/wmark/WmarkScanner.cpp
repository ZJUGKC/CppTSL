﻿/*
** Xin YUAN, 2019, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include "WmarkScanner.h"

#include "base/WmarkDef.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

// WmarkScannerHelper

void WmarkScannerHelper::InitActions(RdScanner& rds)
{
	rds.ClearActions();
	//add
	rds.AddAction(WMARK_SCANNER_TK_ACTION, get_TkAction());
	rds.AddAction(WMARK_SCANNER_COMMENT_ACTION, get_CommentAction());
	rds.AddAction(WMARK_SCANNER_TEXT_ACTION, get_TextAction());
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
