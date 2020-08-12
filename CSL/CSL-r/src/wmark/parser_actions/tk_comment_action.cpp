/*
** Xin YUAN, 2019, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include "../WmarkParser.h"

#include "../base/WmarkDef.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

// TkCommentAction

RdParserAction WmarkParserHelper::get_TkCommentAction(RdParserActionMetaData* pData)
{
	return [pData](const std::string& strToken, std::vector<std::string>& vecError)->bool
			{
				//comment
				assert( pData->posParent.uAddress != 0 );
				RdMetaDataPosition pos = pData->spMeta->InsertAstNode(WMARK_NODETYPE_COMMENT);
				pData->spMeta->SetAstParent(pos, pData->posParent);
				if( pData->posCurrent.uAddress == 0 )
					pData->spMeta->SetAstChild(pData->posParent, pos);
				else
					pData->spMeta->SetAstNext(pData->posCurrent, pos);
				pData->posCurrent = pos;
				return true;
			};
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
