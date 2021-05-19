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

RdParserAction WmarkParserHelper::get_AcceptedAction(RdParserActionMetaData& data)
{
	return [&data](const std::string& strToken, std::vector<std::string>& vecError)->bool
			{
				//symbol
				uint32_t uHash = data.spMeta->CalcHash(WMARK_ROOT_SYMBOL);
				RdMetaDataPosition pos = data.spMeta->InsertSymbol(WMARK_ROOT_SYMBOL, uHash, 0, true);
				RdMetaDataPosition posData = data.spMeta->InsertData(sizeof(RdMetaDataPosition));
				*((RdMetaDataPosition*)(data.spMeta->GetData(posData))) = data.spMeta->GetAstStart();
				data.spMeta->SetData(pos, posData);
				//finish
				data.spMeta->FinishZeroLevel(true);
				return true;
			};
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
