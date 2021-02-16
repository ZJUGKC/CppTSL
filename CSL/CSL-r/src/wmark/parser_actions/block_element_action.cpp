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

RdParserAction WmarkParserHelper::get_BlockElementAction(RdParserActionMetaData& data)
{
	return [&data](const std::string& strToken, std::vector<std::string>& vecError)->bool
			{
				//paragraph
				assert( data.posParent.uAddress != 0 );
				RdMetaDataPosition pos = data.spMeta->InsertAstNode(WMARK_NODETYPE_PARAGRAPH);
				data.spMeta->SetAstParent(pos, data.posParent);
				if( data.posCurrent.uAddress == 0 )
					data.spMeta->SetAstChild(data.posParent, pos);
				else
					data.spMeta->SetAstNext(data.posCurrent, pos);
				//sub tree
				data.posParent = pos;
				data.posCurrent.uAddress = 0;
				return true;
			};
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
