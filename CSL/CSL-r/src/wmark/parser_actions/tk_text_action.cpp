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

// TkTextAction

RdParserAction WmarkParserHelper::get_TkTextAction(RdParserActionMetaData* pData)
{
	return [pData](const std::string& strToken, std::vector<std::string>& vecError)->bool
			{
				//text
				assert( pData->posParent.uAddress != 0 );
				//string
				size_t uSize = strToken.length();
				if( uSize >= (size_t)((std::numeric_limits<uint32_t>::max)()) )
					return false;
				//node
				RdMetaDataPosition pos = pData->spMeta->InsertAstNode(WMARK_NODETYPE_TEXT);
				//token
				RdMetaDataPosition posData = pData->spMeta->InsertData((uint32_t)uSize + 1);
				char* szData = (char*)pData->spMeta->GetData(posData);
				::memcpy(szData, strToken.c_str(), uSize);
				szData[uSize] = '\0';
				//data
				pData->spMeta->SetAstData(pos, posData);
				//link
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
