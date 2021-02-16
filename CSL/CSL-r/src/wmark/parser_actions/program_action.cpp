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

RdParserAction WmarkParserHelper::get_ProgramAction(RdParserActionMetaData& data)
{
	return [&data](const std::string& strToken, std::vector<std::string>& vecError)->bool
			{
				//root
				if( data.posParent.uAddress == 0 && data.posCurrent.uAddress == 0 ) {
					RdMetaDataPosition pos = data.spMeta->InsertAstNode(WMARK_NODETYPE_ROOT);
					RdMetaDataPosition posRoot = data.spMeta->GetAstRoot(data.spMeta->GetAstStart());
					data.spMeta->SetAstParent(pos, posRoot);
					data.spMeta->SetAstChild(posRoot, pos);
					data.posParent = pos;
				}
				return true;
			};
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
