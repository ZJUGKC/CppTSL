/*
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

void WmarkScannerHelper::CreateActions(std::shared_ptr<RdScannerAction>& spTkAction,
									std::shared_ptr<RdScannerAction>& spCommentAction,
									std::shared_ptr<RdScannerAction>& spTextAction)
{
	spTkAction = std::make_shared<RdScannerAction>(get_TkAction());
	spCommentAction = std::make_shared<RdScannerAction>(get_CommentAction());
	spTextAction = std::make_shared<RdScannerAction>(get_TextAction());
}

void WmarkScannerHelper::SetActions(RdScanner& rds,
									const std::shared_ptr<RdScannerAction>& spTkAction,
									const std::shared_ptr<RdScannerAction>& spCommentAction,
									const std::shared_ptr<RdScannerAction>& spTextAction)
{
	rds.ClearActions();
	//add
	rds.AddAction(WMARK_SCANNER_TK_ACTION, spTkAction);
	rds.AddAction(WMARK_SCANNER_COMMENT_ACTION, spCommentAction);
	rds.AddAction(WMARK_SCANNER_TEXT_ACTION, spTextAction);
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
