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

// AcceptedAction

RdParserAction WmarkParserHelper::get_AcceptedAction(RdParserActionMetaData* pData)
{
	return [pData](const std::string& strToken, std::vector<std::string>& vecError)->bool
			{
				//symbol
				RdMetaDataPosition pos = pData->spMeta->InsertSymbol(WMARK_ROOT_SYMBOL, 0, true);
				RdMetaDataPosition posData = pData->spMeta->InsertData(sizeof(RdMetaDataPosition));
				*((RdMetaDataPosition*)(pData->spMeta->GetData(posData))) = pData->spMeta->GetAstStart();
				pData->spMeta->SetData(pos, posData);
				//finish
				pData->spMeta->FinishZeroLevel(true);
				return true;
			};
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
