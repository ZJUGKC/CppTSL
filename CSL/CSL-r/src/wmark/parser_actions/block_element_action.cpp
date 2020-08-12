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

// BlockElementAction

RdParserAction WmarkParserHelper::get_BlockElementAction(RdParserActionMetaData* pData)
{
	return [pData](const std::string& strToken, std::vector<std::string>& vecError)->bool
			{
				//paragraph
				assert( pData->posParent.uAddress != 0 );
				RdMetaDataPosition pos = pData->spMeta->InsertAstNode(WMARK_NODETYPE_PARAGRAPH);
				pData->spMeta->SetAstParent(pos, pData->posParent);
				if( pData->posCurrent.uAddress == 0 )
					pData->spMeta->SetAstChild(pData->posParent, pos);
				else
					pData->spMeta->SetAstNext(pData->posCurrent, pos);
				//sub tree
				pData->posParent = pos;
				pData->posCurrent.uAddress = 0;
				return true;
			};
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
