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

// ProgramAction

RdParserAction WmarkParserHelper::get_ProgramAction(RdParserActionMetaData* pData)
{
	return [pData](const std::string& strToken, std::vector<std::string>& vecError)->bool
			{
				//root
				if( pData->posParent.uAddress == 0 && pData->posCurrent.uAddress == 0 ) {
					RdMetaDataPosition pos = pData->spMeta->InsertAstNode(WMARK_NODETYPE_ROOT);
					RdMetaDataPosition posRoot = pData->spMeta->GetAstRoot(pData->spMeta->GetAstStart());
					pData->spMeta->SetAstParent(pos, posRoot);
					pData->spMeta->SetAstChild(posRoot, pos);
					pData->posParent = pos;
				}
				return true;
			};
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
